#ifndef __RIKERIO_MEMORY_ALLOC_RESPONSE_H__
#define __RIKERIO_MEMORY_ALLOC_RESPONSE_H__

#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class MemoryAlloc : public RPCResponse {

  public:

    MemoryAlloc(Json::Value& result);
    unsigned int get_offset();
    const std::string& get_token();

  private:

    unsigned int offset;
    std::string token;

};

using MemoryAllocPtr = std::shared_ptr<MemoryAlloc>;

}
}
}

#endif
