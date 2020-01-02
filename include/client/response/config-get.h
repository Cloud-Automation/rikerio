#ifndef __RIKERIO_CONFIG_GET_RESPONSE_H__
#define __RIKERIO_CONFIG_GET_RESPONSE_H__

#include "client/response.h"
#include <string>

namespace RikerIO {
namespace Response {
namespace v1 {

class ConfigGet : public RPCResponse {
  public:

    ConfigGet(Json::Value& result) : RPCResponse(result) {

        if (!ok()) {
            return;
        }

        Json::Value data = result["data"];

        profile = data["id"].asString();
        version = data["version"].asString();
        shm_file = data["shm"].asString();
        size = data["size"].asUInt();
        cycle = data["cycle"].asUInt();

    }

    ConfigGet(const std::string& profile,
              const std::string& version,
              const std::string& shm_file,
              unsigned int size,
              unsigned int cycle) :
        RPCResponse(),
        profile(profile),
        version(version),
        shm_file(shm_file),
        size(size),
        cycle(cycle) {

    }


    const std::string& get_profile() {
        return profile;
    }

    const std::string& get_version () {
        return version;
    }

    const std::string& get_shm_file () {
        return shm_file;
    }

    unsigned int get_size () {
        return size;
    }

    unsigned int get_cycle () {
        return cycle;
    }

  private:

    std::string profile = "?";
    std::string version = "?";
    std::string shm_file = "?";
    unsigned int size = 0;
    unsigned int cycle = 0;

};

using ConfigGetPtr = std::shared_ptr<ConfigGet>;

}
}
}

#endif
