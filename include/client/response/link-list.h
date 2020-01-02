#ifndef __RIKERIO_LINK_LIST_RESPONSE_H__
#define __RIKERIO_LINK_LIST_RESPONSE_H__

#include "client/response.h"
#include "client/response/data-list.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class LinkList : public RPCResponse {
  public:

    class LinkListItem {
      public:
        LinkListItem(
            const std::string& key,
            const std::string& id,
            std::shared_ptr<DataList::DataListItem> item) : key(key), id(id), item(item) { }

        const std::string& get_key() const {
            return key;
        }

        const std::string& get_id() const {
            return id;
        }

        std::shared_ptr<DataList::DataListItem> get_item() const {
            return item;
        }

      private:
        const std::string key;
        const std::string id;
        std::shared_ptr<DataList::DataListItem> item;
    };

    LinkList(Json::Value& result) : RPCResponse(result) {

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

    const std::string& get_key() const {
        return key;
    }

    const std::string& get_id() const {
        return id;
    }

    const std::vector<std::shared_ptr<LinkListItem>> get_items() const {
        return items;
    }

  private:

    const std::string key;
    const std::string id;
    std::vector<std::shared_ptr<LinkListItem>> items;

};
using LinkListPtr = std::shared_ptr<LinkList>;
}
}
}


#endif
