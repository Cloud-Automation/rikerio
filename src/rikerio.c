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

static int rio_get_filesize(char* filename, size_t* filesize) {

    struct stat buffer = { };
    if (stat(filename, &buffer) == 0) {
        *filesize = buffer.st_size;
        return 0;
    } else {
        return -1;
    }

}

static int rio_get_memory_pointer(char* filename, char** ptr, size_t size) {

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

static int rio_file_lock(int fd) {
    return flock(fd, LOCK_EX);
}

static int rio_file_unlock(int fd) {
    return flock(fd, LOCK_UN);
}

static int rio_split_string(char* src, char*** trg) {

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

static int rio_mparse(FILE* fp, rio_alloc_entry_t** list) {

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int cntr = 0;


    while ((read = getline(&line, &len, fp)) != -1) {

        char** strList = NULL;
        int ret = 0;

        char tmp[255] = {};
        strncpy(tmp, line, strlen(line) - 1);

        ret = rio_split_string(tmp, &strList);

        free(line);
        line = NULL;

        /* not enough information in this line */

        if (ret != 3) {
            return -1;
        }

        *list = realloc(*list, ++cntr * sizeof(rio_alloc_entry_t));

        (*list)[cntr-1].pid = atoi(strList[0]);
        (*list)[cntr-1].offset = atoi(strList[1]);
        (*list)[cntr-1].size = atoi(strList[2]);

        free(strList[0]);
        free(strList[1]);
        free(strList[2]);

        free(strList);

    }

    free(line);

    return cntr;

}

static int rio_aparse(FILE* fp, rio_adr_entry_t** list) {

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int cntr = 0;

    while ((read = getline(&line, &len, fp)) != -1) {

        char** strList = NULL;
        int ret = 0;

        ret = rio_split_string(line, &strList);

        free(line);
        line = NULL;
        len = 0;

        /* not enough information in this line */

        if (ret != 3) {
            return -1;
        }

        *list = realloc(*list, ++cntr * sizeof(rio_adr_entry_t));

        (*list)[cntr-1].pid = atoi(strList[0]);
        (*list)[cntr-1].adr.byteOffset = atoi(strList[1]);
        (*list)[cntr-1].adr.bitOffset = atoi(strList[2]);

        free(strList[0]);
        free(strList[1]);
        free(strList[2]);

        free(strList);

    }

    free(line);

    return cntr;

}

static int rio_mwrite(FILE* fp, rio_alloc_entry_t** list, unsigned int cntr) {

    int length = 0;
    int fd = fileno(fp);

    lseek(fd, 0, SEEK_SET);

    for (unsigned int i = 0; i < cntr; i += 1) {

        int ret = fprintf(fp, "%d;%d;%d\n", (*list)[i].pid, (*list)[i].offset, (*list)[i].size);

        if (ret == -1) {
            return -1;
        }

        length += ret;

    }

    return length;

}

static int rio_awrite(FILE* fp, rio_adr_entry_t** list, const unsigned int cntr) {

    int length = 0;
    int fd = fileno(fp);

    lseek(fd, 0, SEEK_SET);

    for (unsigned int i = 0; i < cntr; i += 1) {

        int ret = fprintf(fp, "%d;%d;%d\n", (*list)[i].pid, (*list)[i].adr.byteOffset, (*list)[i].adr.bitOffset);

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


int rio_memory_alloc(char* id, pid_t p, uint32_t size, char** ptr, uint32_t* offset) {

    int retVal = 0;

    pid_t pid = p == 0 ? getpid() : p;

    /* 1. get filesize of shm */

    char shmFile[255];
    char allocFile[255];

    sprintf(shmFile, "%s/%s/%s", RIO_ROOT_PATH, id, RIO_SHM_FILE);
    sprintf(allocFile, "%s/%s/%s", RIO_ROOT_PATH, id, RIO_ALLOC_FILE);

    size_t shmFilesize = 0;

    if (rio_get_filesize(shmFile, &shmFilesize) == -1) {
        return -1;
    }

    /* 2. open file located on /var/www/rikerio/{id}/alloc */

    FILE* fp = fopen(allocFile, "r+");

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    if (rio_file_lock(fd) == -1) {
        return -1;
    }

    /* 2. get memory pointer */

    char* tp = NULL;

    if (rio_get_memory_pointer(shmFile, &tp, shmFilesize) == -1) {
        return -1;
    }

    /* 2. read allocations */

    rio_alloc_entry_t* curList = NULL;

    int cntr = rio_mparse(fp, &curList);

    if (cntr == -1) {
        retVal = -1;
        goto exit;
    }

    rio_alloc_entry_t* newList = calloc(1, (cntr + 1) * sizeof(rio_alloc_entry_t));

    /* 3. fit request */

    uint32_t bufferOffset = 0;
    int inserted = 0;

    for (int i = 0; i < cntr + 1; i += 1) {

        if (inserted) {
            memcpy(&newList[i + inserted], &curList[i], sizeof(rio_alloc_entry_t));
            continue;
        }

        long diff;

        if (i == cntr) {
            diff = shmFilesize - bufferOffset;
        } else {
            diff = curList[i].offset - bufferOffset;
        }

        if (diff >= size && !inserted) {
            rio_alloc_entry_t newEntry = { pid, bufferOffset, size };
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
        goto exit;
    }

    /* 5. write back */

    if (rio_mwrite(fp, &newList, cntr + 1) == -1) {
        retVal = -1;
    }

exit:
    free(curList);
    free(newList);

    rio_file_unlock(fd);
    fclose(fp);

    return retVal;

}

int rio_memory_get(char* id, char** ptr, size_t* size) {

    /* 1. get filesize of shm */

    char shmFile[255];
    sprintf(shmFile, "%s/%s/%s", RIO_ROOT_PATH, id, RIO_SHM_FILE);

    /* 2. get filesize */

    size_t shmFilesize = 0;

    if (rio_get_filesize(shmFile, &shmFilesize) == -1) {
        return -1;
    }

    *size = shmFilesize;

    /* 3. get memory pointer */

    char* tp = NULL;

    if (rio_get_memory_pointer(shmFile, &tp, shmFilesize) == -1) {
        return -1;
    }

    *ptr = tp;

    return 0;

}

int rio_memory_free(char* id, pid_t p, uint32_t offset) {

    int retVal = 0;

    /* 0. get process id */

    pid_t pid = p == 0 ? getpid() : p;

    /* set allocation filename */

    char allocFile[255];

    sprintf(allocFile, "%s/%s/%s", RIO_ROOT_PATH, id, RIO_ALLOC_FILE);

    /* 2. open and lock file located on /var/www/rikerio/{id}/alloc */

    FILE* fp = fopen(allocFile, "r+");

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    if (rio_file_lock(fd) == -1) {
        return -1;
    }

    /* 3. read allocations */

    rio_alloc_entry_t* curList = NULL;

    int cntr = rio_mparse(fp, &curList);

    if (cntr <= 0) {
        retVal = -1;
        goto exit;
    }

    rio_alloc_entry_t* newList = calloc(1, (cntr - 1) * sizeof(rio_alloc_entry_t));

    /* 4. fit request */

    int removed = 0;

    for (int i = 0; i < cntr; i += 1) {

        if (curList[i].offset == offset && curList[i].pid == pid) {
            removed += 1;
            continue;
        }

        memcpy(&newList[i-removed], &curList[i], sizeof(rio_alloc_entry_t));

    }

    if (removed == 0) {
        retVal = -1;
        goto exit;
    }

    /* 5. write back */

    if (rio_mwrite(fp, &newList, cntr - 1) == -1) {
        retVal = -1;
    }


exit:

    free(curList);
    free(newList);
    rio_file_unlock(fd);
    fclose(fp);

    return retVal;

}

int rio_memory_freeall(char* id, pid_t p) {

    int retVal = 0;

    /* 0. get process id */

    pid_t pid = p == 0 ? getpid() : p;

    /* set allocation filename */

    char allocFile[255];

    sprintf(allocFile, "%s/%s/%s", RIO_ROOT_PATH, id, RIO_ALLOC_FILE);

    /* 2. open and lock file located on /var/www/rikerio/{id}/alloc */

    FILE* fp = fopen(allocFile, "r+");

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    if (rio_file_lock(fd) == -1) {
        return -1;
    }

    /* 3. read allocations */

    rio_alloc_entry_t* curList = NULL;

    int cntr = rio_mparse(fp, &curList);

    if (cntr <= 0) {
        retVal = -1;
        goto exit;
    }

    rio_alloc_entry_t* newList = calloc(1, (cntr - 1) * sizeof(rio_alloc_entry_t));

    /* 4. fit request */

    int removed = 0;

    for (int i = 0; i < cntr; i += 1) {

        if (curList[i].pid == pid) {
            removed += 1;
            continue;
        }

        memcpy(&newList[i-removed], &curList[i], sizeof(rio_alloc_entry_t));

    }

    if (removed == 0) {
        retVal = -1;
        goto exit;
    }

    /* 5. write back */

    int newLength = rio_mwrite(fp, &newList, cntr - 1);

    if (newLength == -1) {
        retVal = -1;
        goto exit;
    }

    if (ftruncate(fd, newLength) == -1) {
        retVal = 1;
    };

exit:

    free(curList);
    free(newList);
    rio_file_unlock(fd);
    fclose(fp);

    return retVal;

}


int rio_adr_add(char* id, pid_t p, rio_key_t key, rio_adr_t* adr) {

    int retVal = 0;

    /* 0. get process id */

    pid_t pid = p == 0 ? getpid() : p;

    /* 1. get filesize of shm */

    char shmFile[255];
    char linkFile[255];

    sprintf(shmFile, "%s/%s/%s", RIO_ROOT_PATH, id, RIO_SHM_FILE);
    sprintf(linkFile, "%s/%s/links/%s", RIO_ROOT_PATH, id, key);

    size_t shmFilesize = 0;

    if (rio_get_filesize(shmFile, &shmFilesize) == -1) {
        return -1;
    }

    if (adr->byteOffset >= shmFilesize) {
        return -1;
    }

    /* 2. open file located on /var/www/rikerio/{id}/links/{key} */

    FILE* fp = fopen(linkFile, "r+");

    if (!fp) {
        fp = fopen(linkFile, "w+");
    }

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    if (rio_file_lock(fd) == -1) {
        return -1;
    }

    /* 3. read allocations */

    rio_adr_entry_t* curList = NULL;

    int cntr = rio_aparse(fp, &curList);

    if (cntr == -1) {
        retVal = -1;
        goto exit;
    }

    /* 4. check entries */

    int found = 0;

    for (int i = 0; i < cntr; i += 1) {

        if (curList[i].adr.byteOffset == adr->byteOffset &&
                curList[i].adr.bitOffset == adr->bitOffset) {
            found = 1;
            break;
        }

    }

    if (found) {
        retVal = 0;
        goto exit;
        return 0;
    }

    /* 5. write back */

    rio_adr_entry_t adrE = {
        pid, *adr
    };

    int ret = fprintf(fp, "%d;%d;%d\n", adrE.pid, adrE.adr.byteOffset, adrE.adr.bitOffset);

    if (ret == -1) {
        retVal = -1;
    }

exit:

    free(curList);
    rio_file_unlock(fd);
    fclose(fp);

    return retVal;

}

int rio_adr_count(const char* id, pid_t p, rio_key_t key, unsigned int* count) {

    if (count == NULL) {
        return -1;
    }

    int retVal = 0;
    unsigned int retCntr = 0;

    /* 0. get process id */


    pid_t pid = p == 0 ? getpid() : p;

    /* 1. get filesize of shm */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/links/%s", RIO_ROOT_PATH, id, key);

    /* 2. open file located on /var/www/rikerio/{id}/links/{key} */

    FILE* fp = fopen(linkFile, "r");

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    if (rio_file_lock(fd) == -1) {
        return -1;
    }

    /* 3. read allocations */

    rio_adr_entry_t* curList = NULL;

    int cntr = rio_aparse(fp, &curList);

    if (cntr == -1) {
        retVal = -1;
        goto exit;
    }

    /* 4. return */

    unsigned int newCntr = 0;

    for (int i = 0; i < cntr; i += 1) {

        if (pid != -1 && curList[i].pid != pid) {
            continue;
        }

        retCntr += 1;


    }

    *count = retCntr;

    retVal = newCntr;

exit:

    free(curList);
    rio_file_unlock(fd);
    fclose(fp);

    return retVal;


}

int rio_adr_get(const char* id, pid_t p, rio_key_t key, rio_adr_t adr[]) {

    int retVal = 0;

    /* 0. get process id */

    pid_t pid = p == 0 ? getpid() : p;

    /* 1. get filesize of shm */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/links/%s", RIO_ROOT_PATH, id, key);

    /* 2. open file located on /var/www/rikerio/{id}/links/{key} */

    FILE* fp = fopen(linkFile, "r");

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    if (rio_file_lock(fd) == -1) {
        return -1;
    }

    /* 3. read allocations */

    rio_adr_entry_t* curList = NULL;

    int cntr = rio_aparse(fp, &curList);

    if (cntr == -1) {
        retVal = -1;
        goto exit;
    }

    /* 4. return */

    unsigned int newCntr = 0;

    for (int i = 0; i < cntr; i += 1) {

        if (pid != -1 && curList[i].pid != pid) {
            continue;
        }

        memcpy(&adr[newCntr++], &curList[i].adr, sizeof(rio_adr_t));

    }

    retVal = newCntr;

exit:

    free(curList);
    rio_file_unlock(fd);
    fclose(fp);

    return retVal;

}


int rio_link_get(char* id, rio_key_t** linkList) {

    /* create link folder */

    char linksFolder[255];

    sprintf(linksFolder, "%s/%s/links", RIO_ROOT_PATH, id);

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

        *linkList = realloc(*linkList, ++cntr * sizeof(rio_key_t));

        memset(&(*linkList)[cntr-1], 0, sizeof(rio_key_t));

        memcpy(&(*linkList)[cntr-1], ent->d_name, strlen(ent->d_name));

    }

    closedir (dir);

    return cntr;

}


int rio_link_del(char* id, pid_t p, rio_key_t key) {

    int retVal = 0;

    /* 0. get process id */

    pid_t pid = p == 0 ? getpid() : p;

    /* 1. get filesize of shm */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/links/%s", RIO_ROOT_PATH, id, key);


    /* 2. open file located on /var/www/rikerio/{id}/links/{key} */

    FILE* fp = fopen(linkFile, "r+");

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    if (rio_file_lock(fd) == -1) {
        return -1;
    }

    /* 3. read allocations */

    rio_adr_entry_t* curList = NULL;

    int cntr = rio_aparse(fp, &curList);

    if (cntr == -1) {
        retVal = -1;
        goto exit;
    }


    /* 4. remove addresses */

    int removed = 0;

    rio_adr_entry_t* newList = NULL;

    for (int i = 0; i < cntr; i += 1) {

        if (curList[i].pid == pid) {
            removed += 1;
            continue;
        }

        newList = realloc(newList, (cntr - removed + 1) * sizeof(rio_adr_entry_t));

        memcpy(&newList[i-removed], &curList[i], sizeof(rio_adr_entry_t));

    }

    if (removed == 0) {
        retVal = -1;
        goto exit;
    }

    /* 5. write back */

    int newLength = rio_awrite(fp, &newList, cntr - removed);

    if (newLength == -1) {
        retVal = -1;
        goto exit;
    }

    if (ftruncate(fd, newLength) == -1) {
        retVal = 1;
    };

exit:

    free(curList);
    free(newList);
    rio_file_unlock(fd);
    fclose(fp);

    return retVal;

}


int rio_link_clear(char* id, pid_t p) {

    int retVal = 0;

    /* 1. get links */

    rio_key_t* keys = NULL;

    int linkCount = rio_link_get(id, &keys);

    if (linkCount == -1) {
        return -1;
    }

    /* 2. ldel pid in every links */

    int delCount = 0;

    for (int i = 0; i < linkCount; i += 1) {

        if (rio_link_del(id, p, keys[i]) == -1) {
            return -1;
        }

        delCount += 1;

    }

    retVal = delCount;

    return retVal;

}

int rio_alias_add(const char* profile, const char* alias, const char* link) {

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

    if (rio_file_lock(fd) == -1) {
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
    rio_file_unlock(fd);
    fclose(fp);

    return retVal;
}

int rio_alias_rm(const char* profile, const char* alias, const char* link) {

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

    if (rio_file_lock(fd) == -1) {
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

    rio_file_unlock(fd);
    fclose(fp);
    return retVal;

}

int rio_alias_count(const char* profile, const char* alias, unsigned int* count) {

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
        return -1;
    }

    int fd = fileno(fp);

    if (rio_file_lock(fd) == -1) {
        retVal = -1;
        goto exit;
    }


    /* 2. read lines from the file */

    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, fp) != -1) {


        unsigned int cnt = 0;
        rio_key_t key = { };
        strcpy(key, line);

        if (rio_adr_count(profile, -1, key, &cnt) == -1) {
            retVal = -1;
            goto exit;
        }

        adrCount += cnt;

    }

    *count = adrCount;

    /* 4. close and unlock file */

exit:
    rio_file_unlock(fd);
    fclose(fp);

    return retVal;

}

int rio_alias_get(const char* profile, const char* alias, rio_adr_t list[]) {

    if (alias == NULL || list == NULL) {
        return -1;
    }

    int retVal = 0;

    /* 1. try to open and lock file RIO_PERS_PATH/{profile}/alias/{alias} */

    char linkFile[255];

    sprintf(linkFile, "%s/%s/alias/%s", RIO_PERS_PATH, profile, alias);

    FILE* fp = fopen(linkFile, "r+");

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    if (rio_file_lock(fd) == -1) {
        retVal = -1;
        goto exit;
    }


    /* 2. read lines from the file */

    unsigned int index = 0;
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, fp) != -1) {

        unsigned int cnt = 0;
        rio_key_t key = { };
        strcpy(key, line);

        if (rio_adr_count(profile, -1, key, &cnt) == -1) {
            retVal = -1;
            goto exit;
        }

        rio_adr_t* adrList = calloc(cnt, sizeof(rio_adr_t));

        if (rio_adr_get(profile, -1, key, adrList) == -1) {
            retVal = -1;
            goto exit;
        }

        for (unsigned int i = 0; i < cnt; i += 1) {
            memcpy(&list[index++], &adrList[i], sizeof(rio_adr_t));
        }

    }


    /* 4. close and unlock file */

exit:
    rio_file_unlock(fd);
    fclose(fp);

    return retVal;

}

int rio_sem_get(char* id, key_t* key) {

    /* 1. get filesize of shm */

    char shmKeyFile[255];

    sprintf(shmKeyFile, "%s/%s/%s", RIO_ROOT_PATH, id, RIO_SEM_FILE);

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
