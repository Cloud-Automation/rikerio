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


static long rio_get_filesize(char* filename) {

    struct stat buffer = { };
    if (stat(filename, &buffer) == 0) {
        return buffer.st_size;
    } else {
        return -1;
    }

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

    rio_adr_entry_t* curList = NULL;

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

static int rio_mwrite(FILE* fp, rio_alloc_entry_t** list, int cntr) {

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

static int rio_awrite(FILE* fp, rio_adr_entry_t** list, int cntr) {

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

int rio_plist(rio_key_t** list) {

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

        *list = realloc(*list, ++cntr * sizeof(rio_profile_t));

        memset(&(*list)[cntr-1], 0, sizeof(rio_profile_t));

        memcpy(&(*list)[cntr-1], ent->d_name, strlen(ent->d_name));

    }

    closedir (dir);

    return cntr;

}


int rio_malloc(char* id, pid_t p, uint32_t size, void** ptr, uint32_t* offset) {

    int retVal = 0;

    pid_t pid = p == 0 ? getpid() : p;

    /* 1. get filesize of shm */

    char shmFile[255];
    char allocFile[255];

    sprintf(shmFile, "%s/%s/%s", RIO_ROOT_PATH, id, RIO_SHM_FILE);
    sprintf(allocFile, "%s/%s/%s", RIO_ROOT_PATH, id, RIO_ALLOC_FILE);

    long shmFilesize = rio_get_filesize(shmFile);

    if (shmFilesize == -1) {
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

    for (unsigned int i = 0; i < cntr + 1; i += 1) {

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
        retVal == -1;
    }

exit:
    free(curList);
    free(newList);

    rio_file_unlock(fd);
    fclose(fp);

    return retVal;

}

int rio_mget(char* id, void** ptr, uint32_t* size) {

    return 0;

}

int rio_mfree(char* id, pid_t p, uint32_t offset) {

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

    for (unsigned int i = 0; i < cntr; i += 1) {

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
        retVal == -1;
    }


exit:

    free(curList);
    free(newList);
    rio_file_unlock(fd);
    fclose(fp);

    return retVal;

}

int rio_mfreeall(char* id, pid_t p) {

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

    for (unsigned int i = 0; i < cntr; i += 1) {

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


int rio_aadd(char* id, pid_t p, rio_key_t key, rio_adr_t* adr) {

    int retVal = 0;

    /* 0. get process id */

    pid_t pid = p == 0 ? getpid() : p;

    /* 1. get filesize of shm */

    char shmFile[255];
    char linkFile[255];

    sprintf(shmFile, "%s/%s/%s", RIO_ROOT_PATH, id, RIO_SHM_FILE);
    sprintf(linkFile, "%s/%s/links/%s", RIO_ROOT_PATH, id, key);

    long shmFilesize = rio_get_filesize(shmFile);

    if (shmFilesize == -1) {
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
        retVal == -1;
        goto exit;
    }

    /* 4. check entries */

    int found = 0;

    for (unsigned int i = 0; i < cntr; i += 1) {

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


int rio_aget(char* id, pid_t p, rio_key_t key, rio_adr_t** adr) {

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
        retVal == -1;
        goto exit;
    }

    /* 4. return */

    unsigned int newCntr = 0;

    for (unsigned int i = 0; i < cntr; i += 1) {

        if (pid != -1 && curList[i].pid != pid) {
            continue;
        }

        *adr = realloc(*adr, (++newCntr) * sizeof(rio_adr_t));

        memcpy(&(*adr)[newCntr - 1], &curList[i].adr, sizeof(rio_adr_t));

    }

    retVal = newCntr;

exit:

    free(curList);
    rio_file_unlock(fd);
    fclose(fp);

    return retVal;

}


int rio_llist(char* id, rio_key_t** linkList) {

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


int rio_ldel(char* id, pid_t p, rio_key_t key) {

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
        retVal == -1;
        goto exit;
    }


    /* 4. remove addresses */

    int removed = 0;

    rio_adr_entry_t* newList = NULL;

    for (unsigned int i = 0; i < cntr; i += 1) {

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


int rio_lclear(char* id, pid_t p) {

    int retVal = 0;

    /* 1. get links */

    rio_key_t* keys = NULL;

    int linkCount = rio_llist(id, &keys);

    if (linkCount == -1) {
        return -1;
    }

    /* 2. ldel pid in every links */

    int delCount = 0;

    for (unsigned int i = 0; i < linkCount; i += 1) {

        if (rio_ldel(id, p, keys[i]) == -1) {
            return -1;
        }

        delCount += 1;

    }

    retVal = delCount;

    return retVal;

}

int rio_sget(char* id, key_t* key) {

    /* 1. get filesize of shm */

    char shmKeyFile[255];

    sprintf(shmKeyFile, "%s/%s/%s", RIO_ROOT_PATH, id, RIO_SEM_FILE);

    /* 2. open and read file */

    FILE* fp = fopen(shmKeyFile, "rb");

    if (!fp) {
        return -1;
    }

    int rret = fread(key, sizeof(*key), 1, fp);

    if (rret != sizeof(*key)) {
        fclose(fp);
        return -1;
    }

    fclose(fp);

    return 0;



}
