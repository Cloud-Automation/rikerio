#include "common/config.h"

const std::string RikerIO::Config::BaseFolder = "/var/lib/rikerio";
const std::string RikerIO::Config::SocketFile = "socket";
const std::string RikerIO::Config::ShmFile = "shm";
const std::string RikerIO::Config::LinkFile = "links";

const std::string RikerIO::Config::CreateBasePath() {
    return RikerIO::Config::BaseFolder;
}

const std::string RikerIO::Config::CreateProfilePath (const std::string& profile) {
    return RikerIO::Config::BaseFolder + "/" + profile;
}

const std::string RikerIO::Config::CreateSocketPath(const std::string& profile) {
    return RikerIO::Config::BaseFolder + "/" + profile + "/" + Config::SocketFile;
}

const std::string RikerIO::Config::CreateShmPath(const std::string& profile) {
    return RikerIO::Config::BaseFolder + "/" + profile + "/" + Config::ShmFile;
}

const std::string RikerIO::Config::CreateLinkPath(const std::string& profile) {
    return RikerIO::Config::BaseFolder + "/" + profile + "/" + RikerIO::Config::LinkFile;
}
