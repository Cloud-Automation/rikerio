#include "client/request/memory-dealloc.h"

RikerIO::Request::v1::MemoryDealloc::MemoryDealloc(const std::string& token) : token(token) { }

Json::Value RikerIO::Request::v1::MemoryDealloc::create_params() {

    Json::Value params;
    params["token"] = token;
    return params;

}
