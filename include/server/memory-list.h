#ifndef __RIKERIO_SERVER_MEMORY_LIST_H__
#define __RIKERIO_SERVER_MEMORY_LIST_H__

#include "vector"
#include "server/memory-area.h"

namespace RikerIO {

struct MemoryListResponse {
    std::vector<RikerIO::MemoryAreaPtr> data;
    MemoryListResponse() : data() { }
};


}


#endif
