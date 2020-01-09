#include "client/response/data-list.h"

RikerIO::Response::v1::DataList::DataListItem::DataListItem(
    const std::string& id,
    RikerIO::Type type,
    RikerIO::MemoryPosition offset,
    const int semaphore,
    const bool is_priv,
    uint8_t* memory_ptr) :
    id(id),
    type(type),
    offset(offset),
    is_priv(is_priv),
    data_ptr(memory_ptr + offset.get_byte_offset()),
    semaphore(std::make_shared<RikerIO::Semaphore>(semaphore)) {
}

const std::string& RikerIO::Response::v1::DataList::DataListItem::get_id() const {
    return id;
}

const RikerIO::Type& RikerIO::Response::v1::DataList::DataListItem::get_type() const {
    return type;
}

RikerIO::MemoryPosition& RikerIO::Response::v1::DataList::DataListItem::get_offset() {
    return offset;
}

std::shared_ptr<RikerIO::Semaphore> RikerIO::Response::v1::DataList::DataListItem::get_semaphore() const {
    return semaphore;
}

bool RikerIO::Response::v1::DataList::DataListItem::is_private () const {
    return is_priv;
}

uint8_t* RikerIO::Response::v1::DataList::DataListItem::get_data_ptr() const {
    return data_ptr;
}

RikerIO::Response::v1::DataList::DataList(Json::Value& result) :
    RikerIO::Response::v1::DataList::DataList(result, NULL) { }

RikerIO::Response::v1::DataList::DataList(Json::Value& result, uint8_t* memory_ptr)  : RPCResponse(result) {

    if (!ok()) {
        return;
    }

    for (auto a : result["data"]) {

        std::shared_ptr<DataListItem> item = std::make_shared<DataListItem>(
                a["id"].asString(),
                Type(a["type"].asString()),
                MemoryPosition(a["offset"].asString()),
                a["semaphore"].asInt(),
                a["private"].asBool(),
                memory_ptr);

        items.push_back(item);

    }

}

std::vector<std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem>> RikerIO::Response::v1::DataList::get_items() {
    return items;
}
