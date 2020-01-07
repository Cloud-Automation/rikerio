#include "client/response/memory-list.h"

RikerIO::Response::v1::MemoryList::MemoryListItem::MemoryListItem(
    unsigned int offset,
    unsigned int size,
    int semaphore,
    uint8_t* memory_ptr) :
    offset(offset),
    size(size),
    semaphore(std::make_shared<RikerIO::Semaphore>(semaphore)),
    alloc_ptr(memory_ptr + offset) {

}


RikerIO::Response::v1::MemoryList::MemoryListItem::MemoryListItem(
    unsigned int offset,
    unsigned int size,
    int semaphore) :
    RikerIO::Response::v1::MemoryList::MemoryListItem(offset, size, semaphore, NULL) { }

unsigned int RikerIO::Response::v1::MemoryList::MemoryListItem::get_offset () {
    return offset;
}

unsigned int RikerIO::Response::v1::MemoryList::MemoryListItem::get_size () {
    return size;
}

std::shared_ptr<RikerIO::Semaphore> RikerIO::Response::v1::MemoryList::MemoryListItem::get_semaphore () {
    return semaphore;
}

RikerIO::Response::v1::MemoryList::MemoryList(Json::Value& result) :
    RikerIO::Response::v1::MemoryList(result, NULL) { }

RikerIO::Response::v1::MemoryList::MemoryList(Json::Value& result, uint8_t* memory_ptr) :
    RPCResponse(result) {

    if (!ok()) {
        return;
    }

    for (auto a : result["data"]) {
        std::shared_ptr<MemoryListItem> item = std::make_shared<MemoryListItem>(
                a["offset"].asUInt(),
                a["size"].asUInt(),
                a["semaphore"].asInt(),
                memory_ptr);
        items.push_back(item);
    }

}

std::vector<std::shared_ptr<RikerIO::Response::v1::MemoryList::MemoryListItem>>&
RikerIO::Response::v1::MemoryList::get_items() {
    return items;
}
