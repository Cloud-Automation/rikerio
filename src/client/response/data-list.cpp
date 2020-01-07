#include "client/response/data-list.h"

RikerIO::Response::v1::DataList::DataListItem::DataListItem(
    const std::string& id,
    const Utils::Datatype datatype,
    const unsigned int offset,
    const unsigned int index,
    const unsigned int size,
    const int semaphore,
    const bool is_priv,
    uint8_t* memory_ptr) :
    id(id),
    datatype(datatype),
    offset(offset),
    index(index),
    size(size),
    semaphore(semaphore),
    is_priv(is_priv),
    data_ptr(memory_ptr + offset) { }

const std::string& RikerIO::Response::v1::DataList::DataListItem::get_id() const {
    return id;
}

RikerIO::Utils::Datatype RikerIO::Response::v1::DataList::DataListItem::get_datatype() const {
    return datatype;
}

unsigned int RikerIO::Response::v1::DataList::DataListItem::get_offset() const {
    return offset;
}

unsigned int RikerIO::Response::v1::DataList::DataListItem::get_index() const {
    return index;
}

unsigned int RikerIO::Response::v1::DataList::DataListItem::get_size() const {
    return size;
}

int RikerIO::Response::v1::DataList::DataListItem::get_semaphore() const {
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
                Utils::GetTypeFromString(a["type"].asString()),
                a["offset"].asUInt(),
                a["index"].asUInt(),
                a["size"].asUInt(),
                a["semaphore"].asInt(),
                a["private"].asBool(),
                memory_ptr);

        items.push_back(item);

    }

}

std::vector<std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem>> RikerIO::Response::v1::DataList::get_items() {
    return items;
}
