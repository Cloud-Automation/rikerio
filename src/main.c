#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <grp.h>
#include <dirent.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <systemd/sd-daemon.h>
#include "version.h"

static struct option long_options[] = {
    { "size", required_argument, NULL, 's' },
    { "id", required_argument, NULL, 'i' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { NULL, 0, NULL, 0 }
};

static struct runtime_st {

    pid_t pid;

    int running;

    char id[20];
    char folder[255];
    char groupName[20];

    gid_t gid;

    mode_t dirMode;
    mode_t fileMode;

    struct {
        char folder[255];
        struct {
            int protection;
            int visibility;
            uint32_t size;
            char file[255];
            int fd;
            void* ptr;
        } shm;
        struct {
            char folder[255];
        } links;
        struct {
            char file[255];
        } alloc;
        struct {
            char file[255];
            int fd;
            key_t key;
            int id;
        } semaphore;

    } profile;

} runtime;

static void printHelp() {

    printf("Usage: rikerio OPTIONS\n\n");
    printf("Options:\n");
    printf("\t-s|--size\t\tBytesize of Shared Memory Section, defaults to 4096.\n");
    printf("\t-i|--id\t\tName of the memory profile.\n");
    printf("\t-v|--version\tPrint version.\n");
    printf("\t-h|--help\t\tPrint this help.\n\n");
    printf("Created by Stefan Pöter<rikerio@cloud-automation.de>.\n");

}

static void printVersion() {
    printf("%s\n", VERSION_SHORT);
}

static int parseArguments(int argc, char* argv[]) {

    runtime.dirMode = 0770;
    runtime.fileMode = 0664;

    sprintf(runtime.id, "default");
    sprintf(runtime.groupName, "rikerio");
    sprintf(runtime.folder, "/var/run/rikerio");
    sprintf(runtime.profile.folder, "%s/%s", runtime.folder, runtime.id);
    sprintf(runtime.profile.links.folder, "%s/%s", runtime.profile.folder, "links");
    sprintf(runtime.profile.alloc.file, "%s/%s", runtime.profile.folder, "/alloc");
    sprintf(runtime.profile.semaphore.file, "%s/%s", runtime.profile.folder, "/sem");

    runtime.profile.shm.size = 4096;
    sprintf(runtime.profile.shm.file, "%s/%s", runtime.profile.folder, "/shm");

    runtime.profile.shm.protection = PROT_READ | PROT_WRITE;
    runtime.profile.shm.visibility = MAP_SHARED;

    while (1) {

        int option_index = 0;
        int c = getopt_long(argc, argv, "s::i::hv", long_options, &option_index);

        if (c == -1) {
            break;
        }

        int v;
        char* s;

        switch (c) {
        case 's':
            v = atoi(optarg);
            if (v <= 0) {
                fprintf(stderr, "Size musst be greater than zero.\n");
                return -1;
            }
            if (v >= UINT32_MAX) {
                fprintf(stderr, "Max Size is %d.\n", UINT32_MAX);
                return -1;
            }
            runtime.profile.shm.size = v;
            break;
        case 'i':
            s = optarg;
            if (strlen(s) == 0) {
                fprintf(stderr, "Invalid id.\n");
                return -1;
            }
            sprintf(runtime.id, "%s", s);

            sprintf(runtime.profile.folder, "%s/%s", runtime.folder, runtime.id);
            sprintf(runtime.profile.shm.file, "%s/%s", runtime.profile.folder, "/shm");
            sprintf(runtime.profile.links.folder, "%s/%s", runtime.profile.folder, "links");
            sprintf(runtime.profile.alloc.file, "%s/%s", runtime.profile.folder, "/alloc");
            sprintf(runtime.profile.semaphore.file, "%s/%s", runtime.profile.folder, "/sem");

            break;
        case 'h':
            printHelp();
            return 0;
        case 'v':
            printVersion();
            return 0;
        case '?':
            printHelp();
            return 0;
        }

    }

    return 1;

}

static int applyGroupAndRights(char* path, mode_t mode) {

    /* get group */
    struct group* grp = getgrnam(runtime.groupName);

    if (!grp) {
        fprintf(stderr, "No group '%s' found, create user group '%s'.\n", runtime.groupName, runtime.groupName);
        return EXIT_FAILURE;
    }


    mode_t oldMask = umask(0);

    if (chown(path, -1, grp->gr_gid) != 0) {
        fprintf(stderr, "Error setting group '%s' on '%s'.\n", runtime.groupName, path);
        return -1;
    }

    if (chmod(path, mode) != 0) {
        fprintf(stderr, "Error setting permissions on '%s'.\n", path);
        return -1;
    }

    umask(oldMask);

    return 1;

}

static void checkAndCreateFolder(char* folder) {

    DIR* dir = opendir(folder);
    if (!dir) {
        mkdir(folder, runtime.dirMode);
    } else {
        closedir(dir);
    }

}

static int checkAndCreateFile(char* file, int closeFile) {

    int fd = open (file, O_RDWR | O_CREAT);

    if (fd == -1) {
        fprintf(stderr, "Error creating file %s (%s).\n", runtime.profile.shm.file, strerror(errno));
    }

    if (closeFile && fd != -1) {
        close(fd);
    }

    return fd;

}

void tearDown(int exitCode) {

    printf("Exiting application.\n");

    /* remove shared memory file */

    munmap(runtime.profile.shm.ptr, runtime.profile.shm.size);

    unlink(runtime.profile.shm.file);
    unlink(runtime.profile.semaphore.file);
    unlink(runtime.profile.alloc.file);

    DIR* d = opendir(runtime.profile.links.folder);
    struct dirent* dir = NULL;
    while ((dir = readdir(d)) != NULL) {
        char fName[255] = { };
        sprintf(fName, "%s/%s", runtime.profile.links.folder, dir->d_name);
        unlink(fName);
    }
    closedir(d);

    rmdir(runtime.profile.links.folder);
    rmdir(runtime.profile.folder);

    runtime.running = 0;

}

int main(int argc, char** argv) {
    int pa = parseArguments(argc, argv);

    if (pa == -1) {
        return EXIT_FAILURE;
    } else if (pa == 0) {
        return EXIT_SUCCESS;
    }

    printf("Starting RikerIO Version %s.\n", VERSION_SHORT);

    /* get pid */

    runtime.pid = getpid();

    /* check / create folder */

    checkAndCreateFolder(runtime.folder);
    checkAndCreateFolder(runtime.profile.folder);
    checkAndCreateFolder(runtime.profile.links.folder);
    checkAndCreateFile(runtime.profile.alloc.file, 1);

    runtime.profile.semaphore.fd = checkAndCreateFile(runtime.profile.semaphore.file, 0);

    if (runtime.profile.semaphore.fd == -1) {
        tearDown(EXIT_FAILURE);
    }

    /* set group on folder */
    if (applyGroupAndRights(runtime.folder, runtime.dirMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.folder, runtime.dirMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.links.folder, runtime.dirMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.alloc.file, runtime.fileMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.semaphore.file, runtime.fileMode) != 1) {
        tearDown(EXIT_FAILURE);
    }


    /* create empty file */

    runtime.profile.shm.fd = checkAndCreateFile(runtime.profile.shm.file, 0);

    if (runtime.profile.shm.fd == -1) {
        tearDown(EXIT_FAILURE);
    }

    if (ftruncate(runtime.profile.shm.fd, runtime.profile.shm.size) == -1) {
        fprintf(stderr, "Error tuncating file %s (%s).\n)", runtime.profile.shm.file, strerror(errno));
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.shm.file, runtime.fileMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    close(runtime.profile.shm.fd);

    /* create shared memory */

    runtime.profile.shm.ptr = mmap(NULL, runtime.profile.shm.size, runtime.profile.shm.protection, runtime.profile.shm.visibility, runtime.profile.shm.fd, 0);


    /* semaphore setup */

    /* 1. get pid as sem key */

    runtime.profile.semaphore.key = runtime.pid;
    runtime.profile.semaphore.id = semget(runtime.profile.semaphore.key, 1, IPC_CREAT | IPC_EXCL | runtime.fileMode);

    write(runtime.profile.semaphore.fd, &runtime.profile.semaphore.key, sizeof(runtime.profile.semaphore.key));

    close(runtime.profile.semaphore.fd);

    if (runtime.profile.semaphore.id < 0) {
        fprintf(stderr, "Error creating semaphore.\n");
        tearDown(EXIT_FAILURE);
    }

    if (semctl(runtime.profile.semaphore.id, 0, SETVAL, (int) 1) == -1) {
        fprintf(stderr, "Error setting semaphore to initial value.\n");
        tearDown(EXIT_FAILURE);
    }

    signal(SIGINT, tearDown);

    sd_notify(0, "READY=1");

    runtime.running = 1;

    while (runtime.running) sleep(1);

    return EXIT_SUCCESS;

}
