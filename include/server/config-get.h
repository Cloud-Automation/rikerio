#ifndef __RIKERIO_SERVER_CONFIG_GET_H__
#define __RIKERIO_SERVER_CONFIG_GET_H__

#include "string"

namespace RikerIO {

struct ConfigResponse {
    std::string profile;
    std::string version;
    std::string shmFile;
    unsigned int size;
    unsigned int defaultCycle; // in us

    ConfigResponse() :
        profile(""),
        version(""),
        shmFile(""),
        size(0),
        defaultCycle(0) { }

    ConfigResponse(
        std::string profile,
        std::string version,
        std::string shmFile,
        unsigned int size,
        unsigned int defaultCycle) :
        profile(profile),
        version(version),
        shmFile(shmFile),
        size(size),
        defaultCycle(defaultCycle) { }
};


}

#endif
