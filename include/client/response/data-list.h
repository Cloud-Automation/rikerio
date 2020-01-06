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
                     const unsigned int offset,
                     const unsigned int index,
                     const unsigned int size,
                     const int semaphore,
                     const bool is_priv);

        const std::string& get_id() const;
        Utils::Datatype get_datatype() const;
        unsigned int get_offset() const;
        unsigned int get_index() const;
        unsigned int get_size() const;
        int get_semaphore() const;
        bool is_private () const;

      private:
        const std::string id;
        const Utils::Datatype datatype;
        const unsigned int offset;
        const unsigned int index;
        const unsigned int size;
        const int semaphore;
        const bool is_priv;

    };

    DataList(Json::Value& result);
    std::vector<std::shared_ptr<DataListItem>> get_items();

  private:
    std::vector<std::shared_ptr<DataListItem>> items;
};
using DataListPtr = std::shared_ptr<DataList>;
}
}
}

#endif
