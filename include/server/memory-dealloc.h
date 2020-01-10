
#ifndef __RIKERIO_SERVER_MEMORY_DEALLOC_H__
#define __RIKERIO_SERVER_MEMORY_DEALLOC_H__

#include "string"

namespace RikerIO {

struct MemoryDeallocRequest {
    const std::string token;
    MemoryDeallocRequest(std::string token) : token(token) { }
};


}

#endif
