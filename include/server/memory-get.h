#ifndef __RIKERIO_SERVER_MEMORY_GET_H__
#define __RIKERIO_SERVER_MEMORY_GET_H__

#include "server/memory-area.h"

namespace RikerIO {

struct MemoryGetRequest {
    const unsigned int offset;
    MemoryGetRequest(unsigned int offset) : offset(offset) { }
};

struct MemoryGetResponse {
    RikerIO::MemoryAreaPtr data;
    MemoryGetResponse() :  data() { }
};

}


#endif
