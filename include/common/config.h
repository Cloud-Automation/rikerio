#ifndef __RIKERIO_CONFIG_H__
#define __RIKERIO_CONFIG_H__

#include <string>

namespace RikerIO {

class Config {

  public:

    static const std::string BaseFolder;
    static const std::string SocketFile;
    static const std::string ShmFile;
    static const std::string LinkFile;

    static const std::string CreateProfilePath (const std::string& profile);
    static const std::string CreateSocketPath (const std::string& profile);
    static const std::string CreateShmPath (const std::string& profile);
    static const std::string CreateLinkPath (const std::string& profile);

};

}

#endif
