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
            std::shared_ptr<DataList::DataListItem> item);

        const std::string& get_key() const;
        const std::string& get_id() const;
        std::shared_ptr<DataList::DataListItem> get_data() const;

      private:
        const std::string key;
        const std::string id;
        std::shared_ptr<DataList::DataListItem> data;
    };

    LinkList(Json::Value& result);
    const std::string& get_key() const;
    const std::string& get_id() const;
    const std::vector<std::shared_ptr<LinkListItem>> get_items() const;

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
