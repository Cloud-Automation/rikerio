#ifndef __RIKERIO_SERVER_LINK_ADD_H__
#define __RIKERIO_SERVER_LINK_ADD_H__

#include "string"
#include "vector"

namespace RikerIO {

struct LinkAddRequest {
    const std::string key;
    std::vector<std::string> ids;
    LinkAddRequest(std::string key) : key(key), ids() { }
};

struct LinkAddResponse {
    unsigned int counter;
    LinkAddResponse() : counter(0) { }
};


}


#endif
