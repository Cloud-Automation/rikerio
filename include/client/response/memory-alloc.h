#ifndef __RIKERIO_MEMORY_ALLOC_RESPONSE_H__
#define __RIKERIO_MEMORY_ALLOC_RESPONSE_H__

#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class MemoryAlloc : public RPCResponse {

  public:

    MemoryAlloc(Json::Value& result) : RPCResponse(result) {

        if (!ok()) {
            return;
        }

        Json::Value data = result["data"];

        offset = data["offset"].asUInt();
        token = data["token"].asString();

    }

    unsigned int get_offset() {
        return offset;
    }

    const std::string& get_token() {
        return token;
    }

  private:

    unsigned int offset;
    std::string token;

};

using MemoryAllocPtr = std::shared_ptr<MemoryAlloc>;

}
}
}

#endif
