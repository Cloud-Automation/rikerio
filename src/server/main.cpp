#include "server/server.h"

#include "common/CLI11.h"
#include "common/config.h"

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

mode_t dirMode = 0770;
unsigned int running = 1;

static void signalHandler(int sigNo) {

    (void)(sigNo);

    running = 0;

}

static int applyGroupAndRights(const std::string& path, mode_t& mode) {

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

static void checkAndCreateFolder(const std::string& folder, mode_t& dirMode) {

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
    //printf("Notifying systemd (%s).\n", msg.c_str());
    systemd_notify func = (systemd_notify) sd_notify;
    int retVal = func(0, msg.c_str());

    if (retVal < 0) {
        printf("Error notifying (sd_notify) SystemD (%d).\n", retVal);
    }

    dlclose(systemd_so);

}


void tearDown(int exitCode, const std::string& profile) {

    /* remove shared memory file */

    std::string socketFile = RikerIO::Config::CreateSocketPath(profile);

    if (unlink(socketFile.c_str()) == -1) {
        printf("Error removing socket file %s.\n", strerror(errno));
    }

    systemd_notify("STOPPING=1");

    printf("Exiting application.\n");

    exit(exitCode);

}



int main(int argc, char** argv) {

    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    std::string profile = "default";
    unsigned int size = 4096;
    unsigned int cycle = 10000;

    CLI::App app;

    app.add_option("-i,--id", profile, "Profile ID", "default");
    app.add_option("-s,--size", size, "Shared Memory Bytesize", 4096);
    app.add_option("-c,--cycle", cycle, "Cycletime in us", 10000);

    auto printVersion = [](int /*count*/) {
        printf("%s\n", RIO_VERSION);
        exit(EXIT_SUCCESS);
    };

    app.add_flag_function("-v,--version", printVersion, "Print version");

    CLI11_PARSE(app, argc, argv);

    std::string socketFile = RikerIO::Config::CreateSocketPath(profile);

    checkAndCreateFolder(RikerIO::Config::BaseFolder, dirMode);

    if (applyGroupAndRights(RikerIO::Config::BaseFolder, dirMode) != 1) {
        fprintf(stderr, "Error applying rights to Temp. Root Folder (%s).\n", strerror(errno));
        tearDown(EXIT_FAILURE, profile);
    }

    checkAndCreateFolder(RikerIO::Config::CreateProfilePath(profile), dirMode);

    if (applyGroupAndRights(RikerIO::Config::CreateProfilePath(profile), dirMode) != 1) {
        fprintf(stderr, "Error applying rights Temp. Profle Folder (%s).\n", strerror(errno));
        tearDown(EXIT_FAILURE, profile);
    }


    try {

        jsonrpc::UnixDomainSocketServer socket(socketFile, 1);

        RikerIO::Server server(socket, profile, size, cycle);

        if (server.StartListening()) {

            if (applyGroupAndRights(socketFile, dirMode) != 1) {
                fprintf(stderr, "Error applying rights to Socket File (%s).\n", strerror(errno));
                tearDown(EXIT_FAILURE, profile);
            }


            printf("Server started listening...\n");
            systemd_notify("READY=1");

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

    tearDown(EXIT_SUCCESS, profile);

}
