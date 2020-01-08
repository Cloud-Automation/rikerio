#ifndef __RIKERIO_DATA_LIST_RESPONSE_H__
#define __RIKERIO_DATA_LIST_RESPONSE_H__

#include "common/semaphore.h"
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
                     const bool is_priv,
                     uint8_t* memory_ptr);

        const std::string& get_id() const;
        Utils::Datatype get_datatype() const;
        unsigned int get_offset() const;
        unsigned int get_index() const;
        unsigned int get_size() const;
        std::shared_ptr<Semaphore> get_semaphore() const;
        bool is_private () const;
        uint8_t* get_data_ptr() const;

      private:
        const std::string id;
        const Utils::Datatype datatype;
        const unsigned int offset;
        const unsigned int index;
        const unsigned int size;
        const bool is_priv;

        uint8_t* data_ptr;

        std::shared_ptr<Semaphore> semaphore;

    };

    DataList(Json::Value& result);
    DataList(Json::Value& result, uint8_t* memory_ptr);
    std::vector<std::shared_ptr<DataListItem>> get_items();

  private:
    std::vector<std::shared_ptr<DataListItem>> items;
};
using DataListPtr = std::shared_ptr<DataList>;
}
}
}

#endif
