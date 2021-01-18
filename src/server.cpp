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
#include <dlfcn.h>
#include "rikerio.h"
#include "version.h"


static struct option long_options[] = {
    { "size", required_argument, NULL, 's' },
    { "id", required_argument, NULL, 'i' },
    { "cycle", required_argument, NULL, 'c' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { NULL, 0, NULL, 0 }
};

static struct runtime_st {

    pid_t pid;

    int running;

    uint32_t base_cycle;

    char id[20];
    char folder[255];
    char pFolder[255];

    char groupName[20];

    gid_t gid;

    mode_t dirMode;
    mode_t fileMode;

    struct {
        char folder[255];
        char pFolder[255];
        char info_file[255];
        char change_file[255];

        struct {
            char folder[255];
            char link[255];
        } links;
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
        } data;
        struct {
            char file[255];
        } alloc;
        struct {
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
    printf("\t-i|--id\t\tName of the memory profile (default value = 'default').\n");
    printf("\t-c|--cycle\t\tBase Cycle (defaults to 10000us).\n");
    printf("\t-v|--version\tPrint version.\n");
    printf("\t-h|--help\t\tPrint this help.\n\n");
    printf("Created by Stefan Pöter<rikerio@cloud-automation.de>.\n");

}

static void printVersion() {
    printf("%s\n", VERSION_SHORT);
}

static void systemd_notify(char* msg) {

    typedef int (*systemd_notify)(int code, const char *param);

    void *systemd_so = dlopen("libsystemd.so", RTLD_NOW);

    if (systemd_so == NULL) {
        printf("No libsystemd.so found. No SystemD notification happening.\n");
        return;
    }

    systemd_notify *sd_notify = (systemd_notify*) dlsym(systemd_so, "sd_notify");
    printf("Notifying systemd (%s).\n", msg);
    systemd_notify func = (systemd_notify) sd_notify;
    int retVal = func(0, msg);

    if (retVal < 0) {
        printf("Error notifying (sd_notify) SystemD (%d).\n", retVal);
    }

    dlclose(systemd_so);

}


static int parseArguments(int argc, char* argv[]) {

    runtime.dirMode = 0770;
    runtime.fileMode = 0664;

    runtime.base_cycle = 10000;

    sprintf(runtime.id, "default");
    sprintf(runtime.groupName, "rikerio");
    sprintf(runtime.folder, "/var/run/rikerio");
    sprintf(runtime.pFolder, "/var/lib/rikerio");

    strcat(runtime.profile.folder, runtime.folder);
    strcat(runtime.profile.folder, "/");
    strcat(runtime.profile.folder, runtime.id);

    sprintf(runtime.profile.folder, "%s/%s", runtime.folder, runtime.id);
    sprintf(runtime.profile.pFolder, "%s/%s", runtime.pFolder, runtime.id);
    sprintf(runtime.profile.links.folder, "%s/%s", runtime.profile.pFolder, "links");
    sprintf(runtime.profile.links.link, "%s/%s", runtime.profile.folder, "links");
    sprintf(runtime.profile.data.folder, "%s/%s", runtime.profile.folder, "data");
    sprintf(runtime.profile.alloc.file, "%s/%s", runtime.profile.folder, "alloc");

    runtime.profile.shm.size = 4096;
    sprintf(runtime.profile.shm.file, "%s/%s", runtime.profile.folder, "shm");

    runtime.profile.shm.protection = PROT_READ | PROT_WRITE;
    runtime.profile.shm.visibility = MAP_SHARED;

    while (1) {

        int option_index = 0;
        int c = getopt_long(argc, argv, "s::i::hv", long_options, &option_index);

        if (c == -1) {
            break;
        }

        unsigned int v;
        uint32_t base_cycle_value;
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
        case 'c':
            base_cycle_value = atoi(optarg);
            runtime.base_cycle = base_cycle_value;
            if (base_cycle_value == 0) {
                fprintf(stderr, "Base Cycle musst be bigger than zero.\n");
                return -1;
            }
            break;
        case 'i':
            s = optarg;
            if (strlen(s) == 0) {
                fprintf(stderr, "Invalid id.\n");
                return -1;
            }
            sprintf(runtime.id, "%s", s);

            sprintf(runtime.profile.folder, "%s/%s", runtime.folder, runtime.id);
            sprintf(runtime.profile.pFolder, "%s/%s", runtime.pFolder, runtime.id);
            sprintf(runtime.profile.links.folder, "%s/%s", runtime.profile.pFolder, "links");
            sprintf(runtime.profile.links.link, "%s/%s", runtime.profile.folder, "links");
            sprintf(runtime.profile.info_file, "%s/%s", runtime.profile.folder, "info");
            sprintf(runtime.profile.change_file, "%s/%s", runtime.profile.folder, "last");
            sprintf(runtime.profile.shm.file, "%s/%s", runtime.profile.folder, "shm");
            sprintf(runtime.profile.data.folder, "%s/%s", runtime.profile.folder, "data");
            sprintf(runtime.profile.alloc.file, "%s/%s", runtime.profile.folder, "alloc");

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

    printf("Applying rights to %s ... \n", path);

    /* get group */
    struct group* grp = getgrnam(runtime.groupName);

    if (!grp) {
        printf("no group '%s' found!\n", runtime.groupName);
        return EXIT_FAILURE;
    }


    mode_t oldMask = umask(0);

    if (chown(path, -1, grp->gr_gid) != 0) {
        printf("error setting group!\n");
        return -1;
    }

    if (chmod(path, mode) != 0) {
        printf("error setting permissions!\n");
        return -1;
    }

    umask(oldMask);

    return 1;

}

static void checkAndCreateFolder(char* folder) {

    printf("Checking folder %s ... ", folder);

    DIR* dir = opendir(folder);
    if (!dir) {
        if (mkdir(folder, runtime.dirMode) != 0) {
            printf("error creating (%s).\n", strerror(errno));
        } else {
            printf("created successfully!\n");
        }
    } else {
        printf("found!\n");
        closedir(dir);
    }

}

static int checkAndCreateLink(char* link, char* target) {

    printf("Checking link %s ... ", link);

    DIR* dir = opendir(target);

    if (!dir) {
        printf("error (%s)!\n", strerror(errno));
        return -1;
    }

    if (symlink(target, link) != 0) {
        printf("error creating! (%s)\n", strerror(errno));
        return -1;
    } else {
        printf("created!\n");
        return 0;
    }

}

static int checkAndCreateFile(char* file, int closeFile) {

    printf("Creating file %s ... ", file);

    int fd = open (file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    if (fd == -1) {
        printf("error creating (%s).\n", strerror(errno));
    } else {
        printf("success.\n");
    }

    if (closeFile && fd != -1) {
        close(fd);
    }

    return fd;

}

void tearDown(int exitCode) {

    /* remove shared memory file */

    munmap(runtime.profile.shm.ptr, runtime.profile.shm.size);

    unlink(runtime.profile.info_file);
    unlink(runtime.profile.change_file);
    unlink(runtime.profile.shm.file);
    unlink(runtime.profile.alloc.file);
    unlink(runtime.profile.links.link);

    DIR* d = opendir(runtime.profile.data.folder);
    struct dirent* dir = NULL;
    while ((dir = readdir(d)) != NULL) {
        char fName[255] = { };
        strcat(fName, runtime.profile.data.folder);
        strcat(fName, "/");
        strcat(fName, dir->d_name);
        unlink(fName);
    }
    closedir(d);

    rmdir(runtime.profile.data.folder);
    rmdir(runtime.profile.folder);

    runtime.running = 0;

    systemd_notify("STOPPING=1");

    printf("Exiting application.\n");

    exit(exitCode);

}

int main(int argc, char** argv) {

    int pa = parseArguments(argc, argv);

    if (pa == -1) {
        return EXIT_FAILURE;
    } else if (pa == 0) {
        return EXIT_SUCCESS;
    }

    printf("Starting RikerIO Version %s.\n", VERSION_SHORT);

    RikerIO::Profile profile = { };
    strcpy(profile.id, runtime.id);
    profile.byte_size = runtime.profile.shm.size;
    profile.sem_key = getpid();
    profile.base_cycle = runtime.base_cycle;

    /* get pid */

    runtime.pid = getpid();

    /* check / create folder */

    checkAndCreateFolder(runtime.folder);
    checkAndCreateFolder(runtime.profile.folder);
    checkAndCreateFolder(runtime.profile.data.folder);
    checkAndCreateFile(runtime.profile.info_file, 1);
    checkAndCreateFile(runtime.profile.change_file, 0);
    checkAndCreateFile(runtime.profile.alloc.file, 1);

    checkAndCreateFolder(runtime.pFolder);
    checkAndCreateFolder(runtime.profile.pFolder);
    checkAndCreateFolder(runtime.profile.links.folder);

    /* create semphore file */

    if (checkAndCreateLink(runtime.profile.links.link, runtime.profile.links.folder) == -1) {
        fprintf(stderr, "Error creating alias link (%s).\n", strerror(errno));
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.links.link, runtime.dirMode) != 1) {
        fprintf(stderr, "Error applying rights to alias link (%s).\n", strerror(errno));
        tearDown(EXIT_FAILURE);
    }

    /* set group on folder */
    if (applyGroupAndRights(runtime.folder, runtime.dirMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.pFolder, runtime.dirMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.folder, runtime.dirMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.pFolder, runtime.dirMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.links.folder, runtime.dirMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.data.folder, runtime.dirMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.alloc.file, runtime.fileMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.info_file, runtime.fileMode) != 1) {
        tearDown(EXIT_FAILURE);
    }

    if (applyGroupAndRights(runtime.profile.change_file, runtime.fileMode) != 1) {
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

    profile.sem_key = runtime.pid;
    profile.sem_id = semget(profile.sem_key, 1, IPC_CREAT | IPC_EXCL | runtime.fileMode);

    if (profile.sem_id < 0) {
        fprintf(stderr, "Error creating semaphore.\n");
        tearDown(EXIT_FAILURE);
    }

    if (semctl(profile.sem_id, 0, SETVAL, (int) 1) == -1) {
        fprintf(stderr, "Error setting semaphore to initial value.\n");
        tearDown(EXIT_FAILURE);
    }

    // write profile to info file

    int info_fd = open(runtime.profile.info_file, O_WRONLY);

    write(info_fd, &profile, sizeof(profile));

    close(info_fd);

    signal(SIGTERM, tearDown);
    signal(SIGINT, tearDown);


    systemd_notify("READY=1");

    /* setup rikerios own memory area */

    uint16_t version_major = RIO_VERSION_MAJOR;
    uint16_t version_minor = RIO_VERSION_MINOR;
    uint16_t version_patch = RIO_VERSION_PATCH;

    RikerIO::Allocation rio_alloc;
    RikerIO::DataPoint dp_version_major = {};
    RikerIO::DataPoint dp_version_minor = {};
    RikerIO::DataPoint dp_version_patch = {};

    RikerIO::init(runtime.id, profile);
    RikerIO::alloc(profile, 6, "rikerio-server", rio_alloc);

    RikerIO::Data::set(profile, rio_alloc, "rikerio.version.major", dp_version_major);
    RikerIO::Data::set(profile, rio_alloc, "rikerio.version.minor", dp_version_minor);
    RikerIO::Data::set(profile, rio_alloc, "rikerio.version.patch", dp_version_patch);

    memcpy(rio_alloc.ptr, &version_major, sizeof(uint16_t));
    memcpy(rio_alloc.ptr + 2, &version_minor, sizeof(uint16_t));
    memcpy(rio_alloc.ptr + 4, &version_patch, sizeof(uint16_t));



    runtime.running = 1;

    while (runtime.running) sleep(1);

    systemd_notify("STOPPING=1");

    return EXIT_SUCCESS;

}
