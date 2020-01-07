#ifndef __RIKERIO_MEMORY_ALLOC_RESPONSE_H__
#define __RIKERIO_MEMORY_ALLOC_RESPONSE_H__

#include "common/semaphore.h"
#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class MemoryAlloc : public RPCResponse {

  public:

    MemoryAlloc(Json::Value& result);
    MemoryAlloc(Json::Value& result, uint8_t* memory_ptr);
    unsigned int get_offset();
    const std::string& get_token();
    std::shared_ptr<Semaphore> get_semaphore();

    const uint8_t* get_alloc_ptr();

  private:

    unsigned int offset;
    std::string token;
    std::shared_ptr<Semaphore> semaphore;

    uint8_t* alloc_ptr;

};

using MemoryAllocPtr = std::shared_ptr<MemoryAlloc>;

}
}
}

#endif
