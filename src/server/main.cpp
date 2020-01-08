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

#include "spdlog/spdlog.h"

mode_t dirMode = 0770;
unsigned int running = 1;

void signalHandler(int sigNo) {

    (void)(sigNo);

    running = 0;

}

int applyGroupAndRights(const std::string& path, mode_t& mode) {

    std::string groupName = "rikerio";

    /* get group */
    struct group* grp = getgrnam(groupName.c_str());

    if (!grp) {
        spdlog::error("No group {} found.", groupName);
        return EXIT_FAILURE;
    }


    mode_t oldMask = umask(0);

    if (chown(path.c_str(), -1, grp->gr_gid) != 0) {
        spdlog::error("Error setting group {} on {}.", groupName, path);
        return -1;
    }

    if (chmod(path.c_str(), mode) != 0) {
        spdlog::error("Error setting permissions on {}.", path);
        return -1;
    }

    umask(oldMask);

    return 1;

}

void checkAndCreateFolder(const std::string& folder, mode_t& dirMode) {

    DIR* dir = opendir(folder.c_str());
    if (!dir) {
        mkdir(folder.c_str(), dirMode);
    } else {
        closedir(dir);
    }

}

void systemd_notify(std::string msg) {

    void* systemd_so = dlopen("libsystemd.so", RTLD_NOW);

    typedef int (*systemd_notify)(int code, const char *param);

    if (systemd_so == NULL) {
        spdlog::warn("No libsystemd.so found. No SystemD notification possible.");
        return;
    }

    systemd_notify *sd_notify = (systemd_notify*) dlsym(systemd_so, "sd_notify");
    spdlog::info("Notifying SystemD ({}).", msg);
    systemd_notify func = (systemd_notify) sd_notify;
    int retVal = func(0, msg.c_str());

    if (retVal < 0) {
        spdlog::error("Error notifying (sd_notify) SystemD ({}).", retVal);
    }

    dlclose(systemd_so);

}


void tearDown() {

    systemd_notify("STOPPING=1");

    printf("Exiting application.\n");

}



int main(int argc, char** argv) {

    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    std::string profile = "default";
    unsigned int size = 4096;
    unsigned int cycle = 10000;
    bool debug = false;

    CLI::App app;

    app.add_option("-i,--id", profile, "Profile ID", "default");
    app.add_option("-s,--size", size, "Shared Memory Bytesize", 4096);
    app.add_option("-c,--cycle", cycle, "Cycletime in us", 10000);


    auto printVersion = [](int /*count*/) {
        printf("%s\n", RIO_VERSION);
        exit(EXIT_SUCCESS);
    };

    app.add_flag_function("-v,--version", printVersion, "Print version");
    app.add_flag("-d,--debug", debug, "Print debug messages.");

    CLI11_PARSE(app, argc, argv);

    if (debug) {
        spdlog::set_level(spdlog::level::debug);
    }

    checkAndCreateFolder(RikerIO::Config::BaseFolder, dirMode);

    if (applyGroupAndRights(RikerIO::Config::CreateBasePath(), dirMode) != 1) {
        spdlog::error("Error applying rights to project folder {}.", strerror(errno));
        tearDown();
        return EXIT_FAILURE;
    }

    checkAndCreateFolder(RikerIO::Config::CreateProfilePath(profile), dirMode);

    if (applyGroupAndRights(RikerIO::Config::CreateProfilePath(profile), dirMode) != 1) {
        spdlog::error("Error applying user-/group rights to profile folder {}.", strerror(errno));
        tearDown();
        return EXIT_FAILURE;
    }


    try {
        std::string socketFile = RikerIO::Config::CreateSocketPath(profile);

        jsonrpc::UnixDomainSocketServer socket(socketFile, 1);

        RikerIO::Server server(socket, profile, size, cycle);

        if (server.StartListening()) {

            if (applyGroupAndRights(socketFile, dirMode) != 1) {
                spdlog::error("Error applying rights to socket file {}.", strerror(errno));
                tearDown();
                return EXIT_FAILURE;
            }

            spdlog::info("RikerIO Server Version {}.", RIO_VERSION_STRING);

            systemd_notify("READY=1");

            while (running == 1) {
                sleep(1);
            }

            server.StopListening();

        } else {

            spdlog::error("Error listening on socket file.");

        }

    } catch (jsonrpc::JsonRpcException &e) {
        printf("%s\n", e.what());
    }

    tearDown();

    return EXIT_SUCCESS;

}
