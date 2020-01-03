#include "client/response/memory-list.h"

RikerIO::Response::v1::MemoryList::MemoryListItem::MemoryListItem(unsigned int offset, unsigned int size, int semaphore):
    offset(offset),
    size(size),
    semaphore(semaphore) {}

unsigned int RikerIO::Response::v1::MemoryList::MemoryListItem::get_offset () {
    return offset;
}

unsigned int RikerIO::Response::v1::MemoryList::MemoryListItem::get_size () {
    return size;
}

int RikerIO::Response::v1::MemoryList::MemoryListItem::get_semaphore () {
    return semaphore;
}

RikerIO::Response::v1::MemoryList::MemoryList(Json::Value& result) : RPCResponse(result) {

    if (!ok()) {
        return;
    }

    for (auto a : result["data"]) {
        std::shared_ptr<MemoryListItem> item = std::make_shared<MemoryListItem>(
                a["offset"].asUInt(),
                a["size"].asUInt(),
                a["semaphore"].asInt());
        items.push_back(item);
    }

}

std::vector<std::shared_ptr<RikerIO::Response::v1::MemoryList::MemoryListItem>>&
RikerIO::Response::v1::MemoryList::get_items() {
    return items;
}
