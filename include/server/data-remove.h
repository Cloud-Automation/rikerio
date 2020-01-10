#ifndef __RIKERIO_SERVER_DATA_REMOVE_H__
#define __RIKERIO_SERVER_DATA_REMOVE_H__

#include "string"
#include "common/type.h"
#include "common/mem-position.h"

namespace RikerIO {

struct DataRemoveResponse {
    unsigned int count;
    DataRemoveResponse() : count(0) { }
};

struct DataRemoveRequest {
    const std::string pattern;
    const std::string token;
    DataRemoveRequest(std::string pattern, std::string token) :
        pattern(pattern), token(token) { }
};

}


#endif
