#define _GNU_SOURCE

#include "rikerio.h"

#include "functional"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/sem.h>

#define RIO_MEMORY_SEM_LOCK -1
#define RIO_MEMORY_SEM_UNLOCK 1

using namespace RikerIO;

static const int result_error = -1;
static const int result_ok = 0;

rio_size_t get_bitsizeof(RikerIO::Type type) {

    switch (type) {
    case (Type::UNDEF):
        return 0;
    case (Type::BIT):
        return 1;
    case (Type::BOOL):
        return 8;
    case (Type::UINT8):
        return 8;
    case (Type::INT8):
        return 8;
    case (Type::UINT16):
        return 16;
    case (Type::INT16):
        return 16;
    case (Type::UINT32):
        return 32;
    case (Type::INT32):
        return 32;
    case (Type::UINT64):
        return 64;
    case (Type::INT64):
        return 64;
    case (Type::FLOAT):
        return 32;
    case (Type::DOUBLE):
        return 64;
    case (Type::STRING):
        return 0; // undefined
    }

    return 0;

}

static int _rio_get_memory_pointer(const char* filename, char** ptr, size_t size) {

    if (!filename || !ptr) {
        return -1;
    }

    FILE* fp = fopen(filename, "r+");

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    void* p = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (p == MAP_FAILED) {
        return -1;
    }

    *ptr = (char*) p;

    fclose(fp);

    return 0;
}

int _rio_lock_and_handle(const std::string& filename, int flags, std::function<int(int)> cb) {

    int fd = open(filename.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    if (!fd) {
//        error_code = RIO_ERROR_FILE_NOT_FOUND;
        return RikerIO::result_error;
    }

    if (flock(fd, LOCK_EX) == -1) {
//        error_code = RIO_ERROR_FILE_LOCK;
        return RikerIO::result_error;
    }

    int ret_val = cb(fd);

    if (flock(fd, LOCK_UN) == -1) {
//        error_code = RIO_ERROR_FILE_UNLOCK;
        return -1;
    }

    close(fd);

    return ret_val;

}

int _read_allocations(int fd, std::vector<RikerIO::Allocation>& alloc_list) {

    do {

        RikerIO::Allocation tmp_entry;
        int ret = read(fd, &tmp_entry, sizeof(RikerIO::Allocation));

        if (ret == 0) { // eof
            break;
        }

        alloc_list.push_back(tmp_entry);

    } while (1);

    return RikerIO::result_ok;

}

int _write_allocations(int fd, std::vector<RikerIO::Allocation>& alloc_list) {

    for (auto it = alloc_list.begin(); it != alloc_list.end(); ++it) {
        write(fd, &*it, sizeof(RikerIO::Allocation));
    }

    return RikerIO::result_ok;

}

int RikerIO::init(const std::string& profile_id, Profile& ret_profile) {

    /* create link folder */

    const std::string sys_folder = RikerIO::root_path + "/" + profile_id;
    const std::string profile_info = sys_folder + "/info";
    const std::string shm_file = sys_folder + "/" + shared_memory_filename;

    /* open dir */

    DIR *dir;
    if ((dir = opendir (sys_folder.c_str())) == NULL) {
        return -1;
    }
    closedir (dir);

    /* 1. get profile data */

    int fd = open(profile_info.c_str(), O_RDONLY, S_IRUSR);

    if (fd < 0) {
        return result_error;
    }

    read(fd, &ret_profile, sizeof(RikerIO::Profile));

    close(fd);

    /* 2. get memory pointer */

    if (_rio_get_memory_pointer(shm_file.c_str(), &ret_profile.ptr, ret_profile.byte_size) == -1) {
        return result_error;
    }

    ret_profile.sem_id = semget(ret_profile.sem_key, 0, 0);

    return result_ok;

}

int RikerIO::alloc(const Profile& profile, uint32_t size, const std::string& alloc_id, RikerIO::Allocation& entry) {

    /* 1. get filesize of shm */

    const std::string alloc_file = RikerIO::root_path + "/" + profile.id + "/" + RikerIO::allocation_filename;

    /* 2. open file located on /var/www/rikerio/{id}/alloc */

    return _rio_lock_and_handle(alloc_file, O_RDWR, [&](int fd) {

        /* 1. read allocations */

        std::vector<RikerIO::Allocation> alloc_list;

        _read_allocations(fd, alloc_list);

        /* 2. look for an existing allocation */


        for (auto it = alloc_list.begin() ; it != alloc_list.end(); ++it) {
            auto& alloc_item = *it;
            bool id_match = "" && strcmp(alloc_item.id, alloc_id.c_str()) == 0;
            bool size_match = alloc_item.byte_size == size;
            /* if there is a allocation with said id and size then return this
             * and do not create a new entry */

            if (id_match && size_match) {
                entry = alloc_item;
                entry.ptr = profile.ptr + alloc_item.offset;
                return RikerIO::result_ok;
            } else if (id_match && !size_match) {
                alloc_list.erase(it);
                break;
            }

        }

        uint32_t start = 0;
        uint32_t end = 0;
        bool inserted = false;

        auto it = alloc_list.begin();

        do {

            if (it == alloc_list.end()) {
                end = profile.byte_size;
            } else {
                auto& alloc_item = *it;
                end = alloc_item.offset;
            }

            long diff = end - start;

            if (diff >= size) {
                inserted = true;
                RikerIO::Allocation new_entry = { start, size, "", NULL };
                strcpy(new_entry.id, alloc_id.c_str());
                alloc_list.insert(it == alloc_list.end() ? it : it+1, new_entry);
                new_entry.ptr = profile.ptr + start;
                entry = new_entry;
                break;
            } else {
                if (it != alloc_list.end()) {
                    start = end + (*it).byte_size;
                }
            }

            it++;

        } while (it <= alloc_list.end());

        /* 5. write back */

        _write_allocations(fd, alloc_list);

        return !inserted ? RikerIO::result_error : RikerIO::result_ok;

    });

}

int RikerIO::dealloc(const Profile& profile, const std::string& alloc_id) {

    /* 1. get filesize of shm */

    const std::string alloc_file = RikerIO::root_path + "/" + profile.id + "/" + RikerIO::allocation_filename;

    /* 2. open file located on /var/www/rikerio/{id}/alloc */

    return _rio_lock_and_handle(alloc_file, O_RDWR, [&](int fd) {

        /* 1. read allocations */

        std::vector<RikerIO::Allocation> alloc_list;

        _read_allocations(fd, alloc_list);

        bool erased = false;

        /* 2. look for an existing allocation */

        for (auto it = alloc_list.begin() ; it != alloc_list.end(); ++it) {

            auto& alloc_item = *it;
            bool id_match = "" && strcmp(alloc_item.id, alloc_id.c_str()) == 0;
            /* if there is a allocation with said id and size then return this
             * and do not create a new entry */

            if (id_match) {
                alloc_list.erase(it);
                erased = true;
                break;
            }
        }

        /* 5. write back */

        if (erased) {
            _write_allocations(fd, alloc_list);
        }

        return !erased ? RikerIO::result_error : RikerIO::result_ok;

    });

}


#if 0

int rio_alloc_rm(rio_profile_t profile, uint32_t offset) {

    int retVal = 0;

    /* set allocation filename */

    char allocFile[255];

    sprintf(allocFile, "%s/%s/%s", RIO_ROOT_PATH, profile, RIO_ALLOC_FILE);

    /* 2. open and lock file located on /var/www/rikerio/{id}/alloc */

    FILE* fp = fopen(allocFile, "r+");

    if (!fp) {
        retVal = -1;
        goto exit;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto exit;
    }

    /* 3. read allocations */

    unsigned int allocCount = 0;

    if (_rio_memory_count(fp, &allocCount) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    RikerIO::Allocation* curList = calloc(allocCount, sizeof(RikerIO::Allocation));
    RikerIO::Allocation* newList = calloc(allocCount - 1, sizeof(RikerIO::Allocation));

    unsigned int retSize = 0;

    if (_rio_memory_parse(fp, curList, allocCount, &retSize) == -1) {
        retVal = -1;
        goto release;
    }

    /* 4. fit request */

    int removed = 0;

    for (unsigned int i = 0; i < allocCount; i += 1) {

        if (curList[i].offset == offset) {
            removed += 1;
            continue;
        }

        memcpy(&newList[i - removed], &curList[i], sizeof(RikerIO::Allocation));

    }

    if (removed == 0) {
        retVal = -1;
        goto release;
    }

    /* 5. write back */

    if (_rio_memory_write(fp, newList, allocCount - 1) == -1) {
        retVal = -1;
    }

    _rio_counter_increase(profile);

release:

    free(curList);
    free(newList);

releaseFile:

    _rio_file_unlock(fd);
    fclose(fp);

exit:

    return retVal;

}

int rio_alloc_rmall(rio_profile_t profile) {

    int retVal = 0;

    /* set allocation filename */

    char allocFile[255];

    sprintf(allocFile, "%s/%s/%s", RIO_ROOT_PATH, profile, RIO_ALLOC_FILE);

    /* 2. open and lock file located on /var/www/rikerio/{id}/alloc */

    FILE* fp = fopen(allocFile, "w");

    if (!fp) {
        retVal = -1;
    }

    fclose(fp);

    _rio_counter_increase(profile);

    return retVal;

}

int rio_alloc_count(rio_profile_t profile, unsigned int* memoryCount) {

    int retVal = 0;

    if (memoryCount == NULL) {
        retVal = -1;
        goto exit;
    }

    unsigned int retCntr = 0;

    /* 1. get filesize of shm */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/alloc", RIO_ROOT_PATH, profile);

    /* 2. open file located on /var/www/rikerio/{id}/links/{key} */

    FILE* fp = fopen(linkFile, "r");

    if (!fp) {
        retVal = 0;
        *memoryCount = 0;
        goto exit;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    if (_rio_memory_count(fp, &retCntr) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    /* 3. read allocations */

    *memoryCount = retCntr;

releaseFile:

    _rio_file_unlock(fd);
    fclose(fp);

exit:

    return retVal;

}

int rio_alloc_get(rio_profile_t profile, RikerIO::Allocation list[], unsigned int listSize, unsigned int* retSize) {

    int retVal = 0;

    /* 1. get filesize of shm */

    char allocFile[255];

    sprintf(allocFile, "%s/%s/alloc", RIO_ROOT_PATH, profile);

    /* 2. open file located on /var/www/rikerio/{id}/links/{key} */

    FILE* fp = fopen(allocFile, "r");

    if (!fp) {
        retVal = 0;
        goto exit;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    /* 3. read allocations */

    if (_rio_memory_parse(fp, list, listSize, retSize) == -1) {
        retVal = -1;
        goto releaseFile;
    }

releaseFile:

    _rio_file_unlock(fd);
    fclose(fp);

exit:

    return retVal;

}


int rio_link_adr_add(rio_profile_t profile, rio_link_t key, rio_adr_t adr) {

    int retVal = 0;

    /* 1. get filesize of shm */

    char shmFile[255];
    char linkFile[255];

    sprintf(shmFile, "%s/%s/%s", RIO_ROOT_PATH, profile, RIO_SHM_FILE);
    sprintf(linkFile, "%s/%s/links/%s", RIO_ROOT_PATH, profile, key);

    size_t shmFilesize = 0;

    if (_rio_get_filesize(shmFile, &shmFilesize) == -1) {
        retVal = -1;
        goto exit;
    }

    if (adr.byteOffset >= shmFilesize) {
        retVal = -1;
        goto exit;
    }

    /* 2. open file located on /var/www/rikerio/{id}/links/{key} */

    FILE* fp = fopen(linkFile, "r+");

    if (!fp) {
        fp = fopen(linkFile, "w+");
    }

    if (!fp) {
        retVal = -1;
        goto exit;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = 1;
        goto releaseFile;
    }

    /* 3. read allocations */

    unsigned int adrCount = 0;

    if (_rio_adr_count(fp, &adrCount) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    rio_adr_t* curList = calloc(adrCount, sizeof(rio_adr_t));

    unsigned int retSize = 0;

    if (_rio_adr_parse(fp, curList, adrCount, &retSize) == -1) {
        retVal = -1;
        goto release;
    }

    /* 4. check entries */

    int found = 0;

    for (unsigned int i = 0; i < adrCount; i += 1) {

        if (curList[i].byteOffset == adr.byteOffset &&
                curList[i].bitOffset == adr.bitOffset) {
            found = 1;
            break;
        }

    }

    if (found) {
        retVal = 0;
        goto release;
    }

    /* 5. write back */

    int ret = fprintf(fp, "%d;%d\n", adr.byteOffset, adr.bitOffset);

    if (ret == -1) {
        retVal = -1;
    }

    _rio_counter_increase(profile);

release:

    free(curList);

releaseFile:

    _rio_file_unlock(fd);
    fclose(fp);

exit:
    return retVal;

}

int rio_link_adr_count(rio_profile_t profile, rio_link_t key, unsigned int* count) {

    int retVal = 0;

    if (count == NULL) {
        retVal = -1;
        goto exit;
    }

    unsigned int retCntr = 0;

    /* 1. get filesize of shm */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/links/%s", RIO_ROOT_PATH, profile, key);

    /* 2. open file located on /var/www/rikerio/{id}/links/{key} */

    FILE* fp = fopen(linkFile, "r");

    if (!fp) {
        retVal = 0;
        *count = 0;
        goto exit;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    if (_rio_adr_count(fp, &retCntr) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    /* 3. read allocations */

    *count = retCntr;

releaseFile:

    _rio_file_unlock(fd);
    fclose(fp);

exit:

    return retVal;


}

int rio_link_adr_get(rio_profile_t profile, rio_link_t key, rio_adr_t adr[], unsigned int listSize, unsigned int* retSize) {

    int retVal = 0;

    /* 1. get filesize of shm */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/links/%s", RIO_ROOT_PATH, profile, key);

    /* 2. open file located on /var/www/rikerio/{id}/links/{key} */

    FILE* fp = fopen(linkFile, "r");

    if (!fp) {
        retVal = 0;
        goto exit;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    /* 3. read allocations */

    if (_rio_adr_parse(fp, adr, listSize, retSize) == -1) {
        retVal = -1;
        goto releaseFile;
    }

releaseFile:

    _rio_file_unlock(fd);
    fclose(fp);

exit:

    return retVal;

}

int rio_link_adr_rm(rio_profile_t profile, rio_link_t key, rio_adr_t adr) {

    int retVal = 0;

    /* 1. get filesize of shm */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/links/%s", RIO_ROOT_PATH, profile, key);

    /* 2. open file located on /var/www/rikerio/{id}/links/{key} */

    FILE* fp = fopen(linkFile, "r+");

    if (!fp) {
        retVal = 0;
        goto exit;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    /* 3. read allocations */


    rio_adr_t* adrList = calloc(0, sizeof(rio_adr_t));
    unsigned int bufSize = 10;
    unsigned int retSize = 0;

    while (1) {
        adrList = realloc(adrList, bufSize * sizeof(rio_adr_t));

        if (_rio_adr_parse(fp, adrList, bufSize, &retSize) == -1) {
            retVal = -1;
            goto dealloc;
        }

        if (retSize < bufSize) {
            break;
        }
        bufSize += 10;
    }

    /* remove all adresses from file */

    if (ftruncate(fd, 0) == -1) {
        goto dealloc;
    }

    for (unsigned int i = 0; i < retSize; i += 1) {

        if (adrList[i].byteOffset == adr.byteOffset && adrList[i].bitOffset == adr.bitOffset) {
            continue;
        }

        fprintf(fp, "%d;%d\n", adrList[i].byteOffset, adrList[i].bitOffset);

    }

    _rio_counter_increase(profile);

dealloc:
    free(adrList);

releaseFile:

    _rio_file_unlock(fd);
    fclose(fp);

exit:

    return retVal;

}


int rio_link_count(rio_profile_t profile, unsigned int* linkCount) {

    int retVal = 0;

    if (NULL == linkCount) {
        retVal = -1;
        goto exit;
    }

    /* get link folder */

    char linkFolder[255];

    sprintf(linkFolder, "%s/%s/links", RIO_ROOT_PATH, profile);

    /* open dir */

    DIR *dir;
    if ((dir = opendir (linkFolder)) == NULL) {
        retVal = -1;
        goto exit;
    }

    /* read dir content */

    int cntr = 0;
    struct dirent *ent;

    while ((ent = readdir (dir)) != NULL) {

        if (strcmp(ent->d_name, ".") == 0) {
            continue;
        }

        if (strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        cntr += 1;

    }

    *linkCount = cntr;


exit:
    closedir (dir);
    return retVal;

}

int rio_link_get(rio_profile_t profile, rio_link_t list[], unsigned int listSize, unsigned int* retSize) {

    char linksFolder[255];

    sprintf(linksFolder, "%s/%s/links", RIO_ROOT_PATH, profile);

    /* open dir */

    DIR *dir;
    if ((dir = opendir (linksFolder)) == NULL) {
        return -1;
    }

    /* read dir content */

    unsigned int cntr = 0;
    struct dirent *ent;

    while ((ent = readdir (dir)) != NULL) {

        if (strcmp(ent->d_name, ".") == 0) {
            continue;
        }

        if (strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        if (cntr > listSize) {
            break;
        }

        memcpy(&list[cntr], ent->d_name, strlen(ent->d_name));

        cntr += 1;

    }

    closedir (dir);

    *retSize = cntr;

    return 0;

}


int rio_link_rm(rio_profile_t profile, rio_link_t key) {

    int retVal = 0;

    char linkFile[255];

    sprintf(linkFile, "%s/%s/links/%s", RIO_ROOT_PATH, profile, key);

    if (unlink(linkFile) == -1) {
        return -1;
    };

    _rio_counter_increase(profile);

    return retVal;

}


int rio_link_rmall(rio_profile_t profile) {

    int retVal = 0;

    char linksFolder[255];

    sprintf(linksFolder, "%s/%s/links", RIO_ROOT_PATH, profile);

    /* open dir */

    DIR *dir;
    if ((dir = opendir (linksFolder)) == NULL) {
        return -1;
    }

    /* read dir content */

    struct dirent *ent;

    while ((ent = readdir (dir)) != NULL) {

        if (strcmp(ent->d_name, ".") == 0) {
            continue;
        }

        if (strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        char linkFile[255] = { 0 };

        strcat(linkFile, linksFolder);
        strcat(linkFile, "/");
        strcat(linkFile, ent->d_name);

        if (unlink(linkFile) == -1) {
            retVal = -1;
            continue;
        }

    }

    closedir (dir);

    _rio_counter_increase(profile);

    return retVal;

}

int rio_alias_link_add(rio_profile_t profile, rio_alias_t alias, rio_link_t link) {

    int created = 0;
    int found = 0;
    int retVal = 0;

    /* 1. try to open and lock file RIO_PERS_PATH/{profile}/alias/{alias} */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/alias/%s", RIO_PERS_PATH, profile, alias);

    FILE* fp = fopen(linkFile, "r+");

    if (!fp) {
        fp = fopen(linkFile, "w");
        created = 1;
    }

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto exit;
    }

    /* 2. read lines from the file */

    if (!created) {
        found = 0;

        char* line = NULL;
        size_t len = 0;
        while (getline(&line, &len, fp) != -1) {
            if (strncmp(line, link, strlen(link)) == 0) {
                found = 1;
            }
            free(line);
            len = 0;
        }
        free(line);
    }

    /* 3. eventually add new line */

    if (!found) {

        int ret = fprintf(fp, "%s\n", link);

        if (ret == -1) {
            retVal = -1;
            goto exit;
        }

    }

    _rio_counter_increase(profile);

    /* 4. close and unlock file */

exit:
    _rio_file_unlock(fd);
    fclose(fp);

    return retVal;
}

int rio_alias_link_rm(rio_profile_t profile, rio_alias_t alias, rio_link_t link) {

    int retVal = 0;

    /* 1. try to open and lock file RIO_PERS_PATH/{profile}/alias/{alias} */

    char linkFile[255] = { };

    sprintf(linkFile, "%s/%s/alias/%s", RIO_PERS_PATH, profile, alias);

    FILE* fp = fopen(linkFile, "r+");

    if (fp == NULL) {
        return 0;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto exit;
    }

    /* 2. read lines from the file */

    rio_link_t* readLinks = calloc(0, sizeof(rio_link_t));
    unsigned int bufSize = 0;
    char* line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1 && line != NULL) {

        if (strlen(line) == 0) {
            free(line);
            len = 0;
            continue;
        }

        bufSize += 1;
        readLinks = realloc(readLinks, bufSize * sizeof(rio_link_t));
        strcpy(readLinks[bufSize - 1], line);

        free(line);
        len = 0;

    }

    free(line);

    /* 3. reset file */

    if (ftruncate(fd, 0) == -1) {
        retVal = -1;
        goto dealloc;
    }

    /* 4 write file */

    rio_link_t compLink;
    sprintf(compLink, "%s\n", link);
    unsigned int lineCount = 0;

    for (unsigned int i = 0; i < bufSize; i += 1) {

        if (strcmp(readLinks[i], compLink) == 0) {
            continue;
        }

        lineCount += 1;
        if (fprintf(fp, "%s", readLinks[i]) < 0) {
            retVal = -1;
            goto dealloc;
        };

    }

    if (lineCount == 0) {
        unlink(linkFile);
        goto dealloc;
    }

    _rio_counter_increase(profile);

dealloc:
    free(readLinks);

exit:

    _rio_file_unlock(fd);
    fclose(fp);
    return retVal;

}

int rio_alias_adr_count(rio_profile_t profile, rio_alias_t alias, unsigned int* count) {

    if (alias == NULL || count == NULL) {
        return -1;
    }

    int adrCount = 0;
    int retVal = 0;

    /* 1. try to open and lock file RIO_PERS_PATH/{profile}/alias/{alias} */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/alias/%s", RIO_PERS_PATH, profile, alias);

    FILE* fp = fopen(linkFile, "r+");

    if (!fp) {
        retVal = 0;
        *count = 0;
        goto exit;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto releaseFile;
    }


    /* 2. read lines from the file */

    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, fp) != -1) {

        unsigned int cnt = 0;
        rio_link_t key = { };
        strncpy(key, line, strlen(line) - 1);

        free(line);

        if (rio_link_adr_count(profile, key, &cnt) == -1) {
            continue;
        }

        adrCount += cnt;

    }

    *count = adrCount;


releaseFile:

    /* 4. close and unlock file */

    _rio_file_unlock(fd);
    fclose(fp);

exit:

    return retVal;

}

int rio_alias_adr_get(rio_profile_t profile, rio_alias_t alias, rio_adr_t list[], unsigned int listSize, unsigned int* retSize) {

    if (alias == NULL || list == NULL) {
        return -1;
    }

    int retVal = 0;

    /* 1. try to open and lock file RIO_PERS_PATH/{profile}/alias/{alias} */

    char aliasFile[255] = { 0 };

    sprintf(aliasFile, "%s/%s/alias/%s", RIO_PERS_PATH, profile, alias);

    FILE* fp = fopen(aliasFile, "r+");

    if (!fp) {
        retVal = -1;
        goto exit;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto releaseFile;
    }


    /* 2. read lines from the file */

    unsigned int index = 0;
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, fp) != -1) {

        unsigned int cnt = 0;
        rio_link_t key = { };
        strncpy(key, line, strlen(line) - 1);

        if (rio_link_adr_count(profile, key, &cnt) == -1) {
            retVal = -1;
            goto releaseFile;
        }

        rio_adr_t* adrList = calloc(cnt, sizeof(rio_adr_t));

        unsigned int retSize = 0;

        if (rio_link_adr_get(profile, key, adrList, cnt, &retSize) == -1) {
            continue;
        }

        for (unsigned int i = 0; i < cnt; i += 1) {
            if (index > listSize) {
                break;
            }
            memcpy(&list[index++], &adrList[i], sizeof(rio_adr_t));
        }

        if (index > listSize) {
            break;
        }

    }

    *retSize = index;


releaseFile:

    /* 4. close and unlock file */

    _rio_file_unlock(fd);
    fclose(fp);

exit:

    return retVal;

}

int rio_sem_get(rio_profile_t profile, int* semId) {

    if (!semId) {
        return -1;
    }

    /* 1. get filesize of shm */

    char shmKeyFile[255];

    sprintf(shmKeyFile, "%s/%s/%s", RIO_ROOT_PATH, profile, RIO_SEM_FILE);

    key_t key = { 0 };

    /* 2. open and read file */

    FILE* fp = fopen(shmKeyFile, "rb");

    if (!fp) {
        return -1;
    }

    int rret = fread(&key, sizeof(key_t), 1, fp);

    if (rret != 1) {
        fclose(fp);
        return -1;
    }

    *semId = semget(key, 0, 0);

    fclose(fp);

    return 0;

}

int rio_sem_lock(int semId) {

    struct sembuf semaphore = { 0 };

    semaphore.sem_op = RIO_MEMORY_SEM_LOCK;
    semaphore.sem_flg = SEM_UNDO;
    if (semop(semId, &semaphore, 1) == -1) {
        return -1;
    }

    return 1;
}

int rio_sem_unlock(int semId) {

    struct sembuf semaphore = { 0 };

    semaphore.sem_op = RIO_MEMORY_SEM_UNLOCK;
    semaphore.sem_flg = SEM_UNDO;
    if (semop(semId, &semaphore, 1) == -1) {
        return -1;
    }

    return 1;

}

#endif
