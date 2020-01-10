#ifndef __RIKERIO_SERVER_DATA_ADD_H__
#define __RIKERIO_SERVER_DATA_ADD_H__

#include "string"
#include "common/type.h"
#include "common/mem-position.h"

namespace RikerIO {

struct DataAddRequest {
    const std::string id;
    const std::string token;
    const RikerIO::Type type;
    RikerIO::MemoryPosition offset;
    DataAddRequest(
        std::string id,
        std::string token,
        std::string type,
        std::string offset) :
        id(id), token(token), type(type), offset(offset) { }
};

struct DataAddResponse {
    RikerIO::Type type;
    RikerIO::MemoryPosition offset;
    int semaphore;
    DataAddResponse() : type("1bit"), offset("0.0"), semaphore(-1) { }
};


}


#endif
