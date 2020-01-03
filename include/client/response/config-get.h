#ifndef __RIKERIO_CONFIG_GET_RESPONSE_H__
#define __RIKERIO_CONFIG_GET_RESPONSE_H__

#include "client/response.h"
#include <string>

namespace RikerIO {
namespace Response {
namespace v1 {

class ConfigGet : public RPCResponse {
  public:

    ConfigGet(Json::Value& result);

    const std::string& get_profile();
    const std::string& get_version ();
    const std::string& get_shm_file ();
    unsigned int get_size ();
    unsigned int get_cycle ();

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
