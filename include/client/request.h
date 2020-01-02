#ifndef __RIKERIO_CLIENT_REQUEST_H__
#define __RIKERIO_CLIENT_REQUEST_H__

#include <memory>
#include <json/json.h>

namespace RikerIO {

template <unsigned int VERSION>
class RPCRequest {
  public:
    RPCRequest() : version(VERSION) { }
    unsigned int get_version() {
        return version;
    }

    virtual Json::Value create_params() {
        return Json::objectValue;
    }

  private:
    const unsigned int version;
};

}

#endif
