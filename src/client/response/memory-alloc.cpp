#include "client/response/memory-alloc.h"

RikerIO::Response::v1::MemoryAlloc::MemoryAlloc(Json::Value& result, uint8_t* memory_ptr) :
    RPCResponse(result) {

    if (!ok()) {
        return;
    }

    Json::Value data = result["data"];

    offset = data["offset"].asUInt();
    token = data["token"].asString();

    int sem_id = data["semaphore"].asInt();

    semaphore = std::make_shared<Semaphore>(sem_id);


    alloc_ptr = memory_ptr + offset;

}

RikerIO::Response::v1::MemoryAlloc::MemoryAlloc(Json::Value& result) :
    MemoryAlloc(result, NULL) {

}

unsigned int RikerIO::Response::v1::MemoryAlloc::get_offset() {
    return offset;
}

const std::string& RikerIO::Response::v1::MemoryAlloc::get_token() {
    return token;
}

std::shared_ptr<RikerIO::Semaphore> RikerIO::Response::v1::MemoryAlloc::get_semaphore() {
    return semaphore;
}

const uint8_t* RikerIO::Response::v1::MemoryAlloc::get_alloc_ptr() {
    return alloc_ptr;
}
