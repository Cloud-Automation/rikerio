#define _GNU_SOURCE

#include "rikerio.h"
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

static int _rio_get_filesize(char* filename, size_t* filesize) {

    struct stat buffer = { };
    if (stat(filename, &buffer) == 0) {
        *filesize = buffer.st_size;
        return 0;
    } else {
        return -1;
    }

}

static int _rio_get_memory_pointer(char* filename, char** ptr, size_t size) {

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

    *ptr = p;

    fclose(fp);

    return 0;
}

static int _rio_file_lock(int fd) {
    return flock(fd, LOCK_EX);
}

static int _rio_file_unlock(int fd) {
    return flock(fd, LOCK_UN);
}

static int _rio_split_string(char* src, char*** trg) {

    char delimiter[] = ";";
    char *ptr;
    int cntr = 0;

    ptr = strtok(src, delimiter);

    while(ptr != NULL) {
        *trg = realloc(*trg, (cntr + 1) * sizeof(char*));
        (*trg)[cntr] = calloc(1, strlen(ptr) + 1);
        strcpy((*trg)[cntr], ptr);

        cntr += 1;

        ptr = strtok(NULL, delimiter);
    }

    return cntr;

}

static int _rio_count_lines(FILE* fp, unsigned int* count) {

    char* line = NULL;
    size_t len = 0;
    int cntr = 0;

    if (fseek(fp, 0, SEEK_SET) != 0) {
        return -1;
    }

    while (getline(&line, &len, fp) != -1) {
        cntr += 1;
    }

    free(line);

    *count = cntr;

    return 0;

}

static int _rio_memory_count(FILE* fp, unsigned int* count) {

    return _rio_count_lines(fp, count);

}

static int _rio_memory_parse(FILE* fp, rio_alloc_entry_t list[]) {

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int cntr = 0;

    if (fseek(fp, 0, SEEK_SET) != 0) {
        return -1;
    }

    while ((read = getline(&line, &len, fp)) != -1) {

        char** strList = NULL;
        int ret = 0;

        char tmp[255] = {};
        strncpy(tmp, line, strlen(line) - 1);

        ret = _rio_split_string(tmp, &strList);

        free(line);
        line = NULL;

        /* not enough information in this line */

        if (ret != 2) {
            return -1;
        }

        list[cntr].offset = atoi(strList[0]);
        list[cntr].size = atoi(strList[1]);

        free(strList[0]);
        free(strList[1]);

        free(strList);

        cntr += 1;

    }

    free(line);

    return cntr;

}

static int _rio_adr_count(FILE* fp, unsigned int* count) {

    return _rio_count_lines(fp, count);

}

static int _rio_adr_parse(FILE* fp, rio_adr_t list[]) {

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int cntr = 0;

    if (fseek(fp, 0, SEEK_SET) != 0) {
        return -1;
    }

    while ((read = getline(&line, &len, fp)) != -1) {

        char** strList = NULL;
        int ret = 0;

        ret = _rio_split_string(line, &strList);

        free(line);
        line = NULL;
        len = 0;

        /* not enough information in this line */

        if (ret != 2) {
            return -1;
        }

        list[cntr].byteOffset = atoi(strList[0]);
        list[cntr].bitOffset = atoi(strList[1]);

        free(strList[0]);
        free(strList[1]);

        free(strList);

        cntr += 1;

    }

    free(line);

    return cntr;

}

static int _rio_memory_write(FILE* fp, rio_alloc_entry_t* list, unsigned int cntr) {

    int length = 0;
    int fd = fileno(fp);

    lseek(fd, 0, SEEK_SET);

    for (unsigned int i = 0; i < cntr; i += 1) {

        int ret = fprintf(fp, "%d;%d\n", list[i].offset, list[i].size);

        if (ret == -1) {
            return -1;
        }

        length += ret;

    }

    return length;

}

static int _rio_adr_write(FILE* fp, rio_adr_t* list, const unsigned int cntr) {

    int length = 0;
    int fd = fileno(fp);

    lseek(fd, 0, SEEK_SET);

    for (unsigned int i = 0; i < cntr; i += 1) {

        int ret = fprintf(fp, "%d;%d\n", list[i].byteOffset, list[i].bitOffset);

        if (ret == -1) {
            return -1;
        }

        length += ret;

    }

    return length;

}

int rio_profile_count(unsigned int* count) {

    if (!count) {
        return -1;
    }

    unsigned int fcount = 0;

    char sysFolder[255];
    sprintf(sysFolder, "%s", RIO_ROOT_PATH);

    DIR* dirp = opendir(sysFolder);

    if (dirp == NULL) {
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_DIR) {
            fcount++;
        }
    }

    *count = fcount;

    closedir(dirp);

    return 0;

}

int rio_profile_get(rio_profile_t list[]) {

    /* create link folder */

    char sysFolder[255];

    sprintf(sysFolder, "%s", RIO_ROOT_PATH);

    /* open dir */

    DIR *dir;
    if ((dir = opendir (sysFolder)) == NULL) {
        return -1;
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

        memset(&(*list)[cntr-1], 0, sizeof(rio_profile_t));

        memcpy(&(*list)[cntr-1], ent->d_name, strlen(ent->d_name));

    }

    closedir (dir);

    return cntr;

}


int rio_memory_alloc(rio_profile_t profile, uint32_t size, char** ptr, uint32_t* offset) {

    int retVal = 0;

    /* 1. get filesize of shm */

    char shmFile[255];
    char allocFile[255];

    sprintf(shmFile, "%s/%s/%s", RIO_ROOT_PATH, profile, RIO_SHM_FILE);
    sprintf(allocFile, "%s/%s/%s", RIO_ROOT_PATH, profile, RIO_ALLOC_FILE);

    size_t shmFilesize = 0;

    if (_rio_get_filesize(shmFile, &shmFilesize) == -1) {
        retVal = -1;
        goto exit;
    }

    /* 2. open file located on /var/www/rikerio/{id}/alloc */

    FILE* fp = fopen(allocFile, "r+");

    if (!fp) {
        retVal = -1;
        goto exit;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    /* 2. get memory pointer */

    char* tp = NULL;

    if (_rio_get_memory_pointer(shmFile, &tp, shmFilesize) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    /* 2. read allocations */

    unsigned int allocCount = 0;

    if (_rio_memory_count(fp, &allocCount) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    rio_alloc_entry_t* curList = calloc(allocCount, sizeof(rio_alloc_entry_t));
    rio_alloc_entry_t* newList = calloc(1, (allocCount + 1) * sizeof(rio_alloc_entry_t));

    if (_rio_memory_parse(fp, curList) == -1) {
        retVal = -1;
        goto release;
    }

    /* 3. fit request */

    uint32_t bufferOffset = 0;
    int inserted = 0;

    for (unsigned int i = 0; i < allocCount + 1; i += 1) {

        if (inserted) {
            memcpy(&newList[i + inserted], &curList[i], sizeof(rio_alloc_entry_t));
            continue;
        }

        long diff;

        if (i == allocCount) {
            diff = shmFilesize - bufferOffset;
        } else {
            diff = curList[i].offset - bufferOffset;
        }

        if (diff >= size && !inserted) {
            rio_alloc_entry_t newEntry = { bufferOffset, size };
            memcpy(&newList[i], &newEntry, sizeof(rio_alloc_entry_t));
            inserted = 1;
            *offset = bufferOffset;
            *ptr = tp + bufferOffset;
        } else {
            bufferOffset = curList[i].offset + curList[i].size;
            memcpy(&newList[i + inserted], &curList[i], sizeof(rio_alloc_entry_t));
        }

    }

    if (!inserted) {
        retVal = -1;
        goto release;
    }

    /* 5. write back */

    if (_rio_memory_write(fp, newList, allocCount + 1) == -1) {
        retVal = -1;
    }

release:

    free(curList);
    free(newList);

releaseFile:

    _rio_file_unlock(fd);
    fclose(fp);


exit:

    return retVal;

}

int rio_memory_get(rio_profile_t profile, char** ptr, size_t* size) {

    /* 1. get filesize of shm */

    char shmFile[255];
    sprintf(shmFile, "%s/%s/%s", RIO_ROOT_PATH, profile, RIO_SHM_FILE);

    /* 2. get filesize */

    size_t shmFilesize = 0;

    if (_rio_get_filesize(shmFile, &shmFilesize) == -1) {
        return -1;
    }

    *size = shmFilesize;

    /* 3. get memory pointer */

    char* tp = NULL;

    if (_rio_get_memory_pointer(shmFile, &tp, shmFilesize) == -1) {
        return -1;
    }

    *ptr = tp;

    return 0;

}

int rio_memory_free(rio_profile_t profile, uint32_t offset) {

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

    rio_alloc_entry_t* curList = calloc(allocCount, sizeof(rio_alloc_entry_t));
    rio_alloc_entry_t* newList = calloc(allocCount - 1, sizeof(rio_alloc_entry_t));

    if (_rio_memory_parse(fp, curList) == -1) {
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

        memcpy(&newList[i-removed], &curList[i], sizeof(rio_alloc_entry_t));

    }

    if (removed == 0) {
        retVal = -1;
        goto release;
    }

    /* 5. write back */

    if (_rio_memory_write(fp, newList, allocCount - 1) == -1) {
        retVal = -1;
    }


release:

    free(curList);
    free(newList);

releaseFile:

    _rio_file_unlock(fd);
    fclose(fp);

exit:

    return retVal;

}

int rio_memory_freeall(rio_profile_t profile) {

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

    if (_rio_adr_parse(fp, curList) == -1) {
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

int rio_link_adr_get(rio_profile_t profile, rio_link_t key, rio_adr_t adr[]) {

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

    if (_rio_adr_parse(fp, adr) == -1) {
        retVal = -1;
        goto releaseFile;
    }

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

int rio_link_get(rio_profile_t profile, rio_link_t list[]) {

    char linksFolder[255];

    sprintf(linksFolder, "%s/%s/links", RIO_ROOT_PATH, profile);

    /* open dir */

    DIR *dir;
    if ((dir = opendir (linksFolder)) == NULL) {
        return -1;
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

        memcpy(&list[cntr], ent->d_name, strlen(ent->d_name));

        cntr += 1;

    }

    closedir (dir);

    return 0;

}


int rio_link_rm(rio_profile_t profile, rio_link_t key) {

    int retVal = 0;

    char linkFile[255];

    sprintf(linkFile, "%s/%s/links/%s", RIO_ROOT_PATH, profile, key);

    if (unlink(linkFile) == -1) {
        return -1;
    };

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

        sprintf(linkFile, "%s/%s", linksFolder, ent->d_name);

        if (unlink(linkFile) == -1) {
            retVal = -1;
            continue;
        }

    }

    closedir (dir);

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

        }
    }

    /* 3. eventually add new line */

    if (!found) {

        int ret = fprintf(fp, "%s\n", link);

        if (ret == -1) {
            retVal = -1;
            goto exit;
        }

    }

    /* 4. close and unlock file */

exit:
    _rio_file_unlock(fd);
    fclose(fp);

    return retVal;
}

int rio_alias_link_rm(rio_profile_t profile, rio_alias_t alias, rio_link_t link) {

    int found = 0;
    int retVal = 0;

    /* 1. try to open and lock file RIO_PERS_PATH/{profile}/alias/{alias} */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/alias/%s", RIO_PERS_PATH, profile, alias);

    FILE* fp = fopen(linkFile, "r+");

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto exit;
    }

    /* 2. read and write lines from the file */

    found = 0;

    char* line = NULL;
    size_t len = 0;
    char* buffer = malloc(0);
    size_t curSize = 0;


    while (getline(&line, &len, fp) != -1 && link != NULL) {
        if (strncmp(line, link, strlen(line) - 1) != 0) {
            size_t insertOffset = curSize;
            curSize += strlen(line) + 1;
            buffer = realloc(buffer, curSize);
            memcpy(buffer + insertOffset, line, strlen(line));
        } else {
            found = 1;
        }
    }

    if (link == NULL) {
        found = 1;
    }

    if (!found) {
        retVal = 0;
        free(buffer);
        goto exit;
    }

    if (found && curSize > 0) {
        fseek(fp, 0, SEEK_SET);
        fprintf(fp, "%s", buffer);
        ftruncate(fd, curSize);
        free(buffer);
    }

    if (found && curSize == 0) {
        unlink(linkFile);
        free(buffer);
        goto exit;
    }


    /* 4. close and unlock file */

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

int rio_alias_adr_get(rio_profile_t profile, rio_alias_t alias, rio_adr_t list[]) {

    if (alias == NULL || list == NULL) {
        return -1;
    }

    int retVal = 0;

    /* 1. try to open and lock file RIO_PERS_PATH/{profile}/alias/{alias} */

    char aliasFile[255];

    sprintf(aliasFile, "%s/%s/alias/%s", RIO_PERS_PATH, profile, alias);

    FILE* fp = fopen(aliasFile, "r+");

    if (!fp) {
        retVal = 0;
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
        strncpy(key, line, strlen(line)-1);

        if (rio_link_adr_count(profile, key, &cnt) == -1) {
            retVal = -1;
            goto releaseFile;
        }

        rio_adr_t* adrList = calloc(cnt, sizeof(rio_adr_t));

        if (rio_link_adr_get(profile, key, adrList) == -1) {
            continue;
        }

        for (unsigned int i = 0; i < cnt; i += 1) {
            memcpy(&list[index++], &adrList[i], sizeof(rio_adr_t));
        }

    }



releaseFile:

    /* 4. close and unlock file */

    _rio_file_unlock(fd);
    fclose(fp);

exit:

    return retVal;

}

int rio_sem_get(rio_profile_t profile, key_t* key) {

    /* 1. get filesize of shm */

    char shmKeyFile[255];

    sprintf(shmKeyFile, "%s/%s/%s", RIO_ROOT_PATH, profile, RIO_SEM_FILE);

    /* 2. open and read file */

    FILE* fp = fopen(shmKeyFile, "rb");

    if (!fp) {
        return -1;
    }

    int rret = fread(key, sizeof(key_t), 1, fp);

    if (rret != 1) {
        fclose(fp);
        return -1;
    }

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

int rio_lock(rio_profile_t profile, int* handle) {


    int retVal = 0;

    if (handle == NULL) {
        retVal = -1;
        goto exit;
    }

    /* 1. try to open and lock file RIO_ROOT_PATH/{profile}/sync */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/sync", RIO_ROOT_PATH, profile);

    FILE* fp = fopen(linkFile, "r");

    if (!fp) {
        retVal = -1;
        goto exit;
    }

    int fd = fileno(fp);

    if (_rio_file_lock(fd) == -1) {
        retVal = -1;
        goto releaseFile;
    }

    *handle = fd;

    goto exit;

releaseFile:

    fclose(fp);

exit:

    return retVal;

}

int rio_unlock(int handle) {

    int retVal = 0;

    if (_rio_file_lock(handle) == -1) {
        retVal = -1;
        goto exit;
    }

    close(handle);

exit:
    return retVal;


}
