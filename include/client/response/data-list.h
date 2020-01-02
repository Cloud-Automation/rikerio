#ifndef __RIKERIO_DATA_LIST_RESPONSE_H__
#define __RIKERIO_DATA_LIST_RESPONSE_H__

#include "common/utils.h"
#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class DataList : public RikerIO::RPCResponse {
  public:

    class DataListItem {
      public:

        DataListItem(const std::string& id,
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
            is_priv(is_priv) { };

        const std::string& get_id() const {
            return id;
        }

        Utils::Datatype get_datatype() const {
            return datatype;
        }

        int get_semaphore() const {
            return semaphore;
        }

        unsigned int get_offset() const {
            return offset;
        }

        unsigned int get_index() const {
            return index;
        }

        unsigned int get_size() const {
            return size;
        }

        bool is_private () const {
            return is_priv;
        }

      private:
        const std::string id;
        const Utils::Datatype datatype;
        const int semaphore;
        const unsigned int offset;
        const unsigned int index;
        const unsigned int size;
        const bool is_priv;

    };

    DataList(Json::Value& result)  : RPCResponse(result) {

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

    std::vector<std::shared_ptr<DataListItem>> get_items() {
        return items;
    }

  private:
    std::vector<std::shared_ptr<DataListItem>> items;
};
using DataListPtr = std::shared_ptr<DataList>;
}
}
}

#endif
