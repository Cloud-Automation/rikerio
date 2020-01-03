#include "client/response/memory-alloc.h"

RikerIO::Response::v1::MemoryAlloc::MemoryAlloc(Json::Value& result) : RPCResponse(result) {

    if (!ok()) {
        return;
    }

    Json::Value data = result["data"];

    offset = data["offset"].asUInt();
    token = data["token"].asString();

}

unsigned int RikerIO::Response::v1::MemoryAlloc::get_offset() {
    return offset;
}

const std::string& RikerIO::Response::v1::MemoryAlloc::get_token() {
    return token;
}
