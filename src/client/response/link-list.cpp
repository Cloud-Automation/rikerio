#include "client/response/link-list.h"

RikerIO::Response::v1::LinkList::LinkListItem::LinkListItem(
    const std::string& key,
    const std::string& id,
    std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem> item) : key(key), id(id), item(item) { }

const std::string& RikerIO::Response::v1::LinkList::LinkListItem::get_key() const {
    return key;
}

const std::string& RikerIO::Response::v1::LinkList::LinkListItem::get_id() const {
    return id;
}

std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem>
RikerIO::Response::v1::LinkList::LinkListItem::get_item() const {
    return item;
}

RikerIO::Response::v1::LinkList::LinkList(Json::Value& result) : RPCResponse(result) {

    if (!ok()) {
        return;
    }

    for (auto a : result["data"]) {

        std::shared_ptr<LinkListItem> linkItem;

        if (!a["data"]) {
            linkItem = std::make_shared<LinkListItem>(a["key"].asString(), a["id"].asString(), nullptr);
            items.push_back(linkItem);
        } else {


            Json::Value data = a["data"];

            auto item = std::make_shared<DataList::DataListItem>(
                            data["id"].asString(),
                            Utils::GetTypeFromString(a["type"].asString()),
                            data["offset"].asUInt(),
                            data["index"].asUInt(),
                            data["size"].asUInt(),
                            data["semaphore"].asInt(),
                            data["private"].asBool());

            linkItem = std::make_shared<LinkListItem>(a["key"].asString(), a["id"].asString(), item);

        }

        items.push_back(linkItem);
    }

}

const std::string& RikerIO::Response::v1::LinkList::get_key() const {
    return key;
}

const std::string& RikerIO::Response::v1::LinkList::get_id() const {
    return id;
}

const std::vector<std::shared_ptr<RikerIO::Response::v1::LinkList::LinkListItem>> RikerIO::Response::v1::LinkList::get_items() const {
    return items;
}
