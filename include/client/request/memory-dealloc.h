#ifndef __RIKERIO_MEMORY_DEALLOC_REQUEST_H__
#define __RIKERIO_MEMORY_DEALLOC_REQUEST_H__

#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {
class MemoryDealloc : public RikerIO::RPCRequest<1> {

  public:
    MemoryDealloc(const std::string& token) : token(token) { }

    Json::Value create_params() override {
        Json::Value params;
        params["token"] = token;
        return params;
    }

  private:
    const std::string token;

};
}
}
}

#endif
