#ifndef __RIKERIO_SERVER_MEMORY_ALLOC_H__
#define __RIKERIO_SERVER_MEMORY_ALLOC_H__

#include "string"

namespace RikerIO {

struct MemoryAllocRequest {
    const unsigned int size;
    MemoryAllocRequest(unsigned int size) : size(size) { }
};

struct MemoryAllocResponse {

    unsigned int offset;
    std::string token;
    int semaphore;

    MemoryAllocResponse() :
        offset(0),
        token(""),
        semaphore(-1) { }

    MemoryAllocResponse(unsigned int offset, std::string token, int semaphore) :
        offset(offset),
        token(token),
        semaphore(semaphore) { };

};

}

#endif
