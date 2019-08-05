#include "server.h"
#include <string>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <grp.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <version.h>
#include <dlfcn.h>
#include <signal.h>

std::string profile = "default";
unsigned int size = 4096;
mode_t dirMode = 0770;
unsigned int running = 1;


static struct option long_options[] = {
    { "id", required_argument, NULL, 'i' },
    { "size", required_argument, NULL, 's' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { NULL, 0, NULL, 0 }
};

static void signalHandler(int sigNo) {

    running = 0;

}

static void printHelp() {

    printf("Usage: rio-server [OPTIONS] \n\n");
    printf("Options:\n");
    printf("\t-i|--id\t\tName of the memory profile (defaults to 'default').\n");
    printf("\t-s|--size\tMemory Size (defaults to 4096 Bytes).\n");
    printf("\t-v|--version\tPrint version.\n");
    printf("\t-h|--help\tPrint this help.\n\n");
    printf("Created by Stefan Pöter<rikerio@cloud-automation.de>.\n");

}

static void printVersion() {
    printf("%s\n", VERSION_SHORT);
}


static int parseArguments(int argc, char* argv[]) {

    while (1) {

        int option_index = 0;
        int c = getopt_long(argc, argv, "i::hv", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
        case 'i':
            if (strlen(optarg) == 0) {
                fprintf(stderr, "Invalid id.\n");
                return -1;
            }
            profile = optarg;

            break;
        case 's':
            size = atoi(optarg);
            if (errno == ERANGE) {
                printf("Error parsing size\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'h':
            printHelp();
            exit(EXIT_SUCCESS);
        case 'v':
            printVersion();
            exit(EXIT_SUCCESS);
        case '?':
            printHelp();
            exit(EXIT_SUCCESS);
        }

    }

    return 1;

}

static int applyGroupAndRights(std::string& path, mode_t& mode) {

    std::string groupName = "rikerio";

    /* get group */
    struct group* grp = getgrnam(groupName.c_str());

    if (!grp) {
        fprintf(stderr, "No group '%s' found, create user group '%s'.\n", groupName.c_str(), groupName.c_str());
        return EXIT_FAILURE;
    }


    mode_t oldMask = umask(0);

    if (chown(path.c_str(), -1, grp->gr_gid) != 0) {
        fprintf(stderr, "Error setting group '%s' on '%s'.\n", groupName.c_str(), path.c_str());
        return -1;
    }

    if (chmod(path.c_str(), mode) != 0) {
        fprintf(stderr, "Error setting permissions on '%s'.\n", path.c_str());
        return -1;
    }

    umask(oldMask);

    return 1;

}

static void checkAndCreateFolder(std::string& folder, mode_t& dirMode) {

    DIR* dir = opendir(folder.c_str());
    if (!dir) {
        mkdir(folder.c_str(), dirMode);
    } else {
        closedir(dir);
    }

}

static void systemd_notify(std::string msg) {

    typedef int (*systemd_notify)(int code, const char *param);

    void *systemd_so = dlopen("libsystemd.so", RTLD_NOW);

    if (systemd_so == NULL) {
        printf("No libsystemd.so found. No SystemD notification happening.\n");
        return;
    }

    systemd_notify *sd_notify = (systemd_notify*) dlsym(systemd_so, "sd_notify");
    printf("Notifying systemd (%s).\n", msg.c_str());
    systemd_notify func = (systemd_notify) sd_notify;
    int retVal = func(0, msg.c_str());

    if (retVal < 0) {
        printf("Error notifying (sd_notify) SystemD (%d).\n", retVal);
    }

    dlclose(systemd_so);

}


void tearDown(int exitCode) {

    /* remove shared memory file */

    std::string tmpRootFolder = "/var/run/rikerio";
    std::string perRootFolder = "/var/lib/rikerio";
    std::string tmpProfileFolder = tmpRootFolder + "/" + profile;
    std::string perProfileFolder = perRootFolder + "/" + profile;

    std::string socketFile = tmpProfileFolder + "/socket";

    if (unlink(socketFile.c_str()) == -1) {
        printf("Error removing socket file %s.\n", strerror(errno));
    }

    rmdir(tmpProfileFolder.c_str());
    rmdir(tmpRootFolder.c_str());

    systemd_notify("STOPPING=1");

    printf("Exiting application.\n");

    exit(exitCode);

}



int main(int argc, char** argv) {

    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);


    parseArguments(argc, argv);

    std::string tmpRootFolder = "/var/run/rikerio";
    std::string perRootFolder = "/var/lib/rikerio";
    std::string tmpProfileFolder = tmpRootFolder + "/" + profile;
    std::string perProfileFolder = perRootFolder + "/" + profile;

    std::string socketFile = tmpProfileFolder + "/socket";

    checkAndCreateFolder(tmpRootFolder, dirMode);

    if (applyGroupAndRights(tmpRootFolder, dirMode) != 1) {
        fprintf(stderr, "Error applying rights to Temp. Root Folder (%s).\n", strerror(errno));
        tearDown(EXIT_FAILURE);
    }

    checkAndCreateFolder(tmpProfileFolder, dirMode);

    if (applyGroupAndRights(tmpProfileFolder, dirMode) != 1) {
        fprintf(stderr, "Error applying rights Temp. Profle Folder (%s).\n", strerror(errno));
        tearDown(EXIT_FAILURE);
    }


    try {

        jsonrpc::UnixDomainSocketServer socket(socketFile, 1);


        RikerIO::Server server(socket, profile);

        if (server.StartListening()) {

            if (applyGroupAndRights(socketFile, dirMode) != 1) {
                fprintf(stderr, "Error applying rights to Socket File (%s).\n", strerror(errno));
                tearDown(EXIT_FAILURE);
            }


            printf("Server started listening...\n");

            while (running == 1) {
                sleep(1);
            }

            server.StopListening();

        } else {

            printf("Error listening\n");

        }



    } catch (jsonrpc::JsonRpcException &e) {
        printf("%s\n", e.what());
    }

    tearDown(EXIT_SUCCESS);

}
