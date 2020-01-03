#include "client/response/data-list.h"


RikerIO::Response::v1::DataList::DataListItem::DataListItem(const std::string& id,
        const Utils::Datatype datatype,
        const int semaphore,
        const unsigned int offset,
        const unsigned int index,
        const unsigned int size,
        const bool is_priv) :
    id(id),
    datatype(datatype),
    semaphore(semaphore),
    offset(offset),
    index(index),
    size(size),
    is_priv(is_priv) { }

const std::string& RikerIO::Response::v1::DataList::DataListItem::get_id() const {
    return id;
}

RikerIO::Utils::Datatype RikerIO::Response::v1::DataList::DataListItem::get_datatype() const {
    return datatype;
}

int RikerIO::Response::v1::DataList::DataListItem::get_semaphore() const {
    return semaphore;
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

bool RikerIO::Response::v1::DataList::DataListItem::is_private () const {
    return is_priv;
}


RikerIO::Response::v1::DataList::DataList(Json::Value& result)  : RPCResponse(result) {

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
                a["private"].asBool());

        items.push_back(item);

    }

}

std::vector<std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem>> RikerIO::Response::v1::DataList::get_items() {
    return items;
}
