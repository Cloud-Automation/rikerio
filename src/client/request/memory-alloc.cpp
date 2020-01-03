#include "client/request/memory-alloc.h"

RikerIO::Request::v1::MemoryAlloc::MemoryAlloc(unsigned int size) : size(size)  {}

Json::Value RikerIO::Request::v1::MemoryAlloc::create_params() {
    Json::Value params;
    params["size"] = size;
    return params;
}
