#ifndef __RIKERIO_MEMORY_DEALLOC_RESPONSE_H__
#define __RIKERIO_MEMORY_DEALLOC_RESPONSE_H__

#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class MemoryDealloc : public RPCResponse {
  public:
    MemoryDealloc(Json::Value& result);
};
using MemoryDeallocPtr = std::shared_ptr<MemoryDealloc>;
}
}
}

#endif
