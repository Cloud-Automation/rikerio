#ifndef __RIKERIO_SERVER_LINK_REMOVE_H__
#define __RIKERIO_SERVER_LINK_REMOVE_H__

#include "string"
#include "vector"

namespace RikerIO {

struct LinkRemoveRequest {
    const std::string pattern;
    std::vector<std::string> ids;
    LinkRemoveRequest(std::string pattern) : pattern(pattern), ids() { }
};

struct LinkRemoveResponse {
    unsigned int counter;
    LinkRemoveResponse() : counter(0) { }
};

}


#endif
