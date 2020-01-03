#ifndef __RIKERIO_MEMORY_LIST_RESPONSE_H__
#define __RIKERIO_MEMORY_LIST_RESPONSE_H__

#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class MemoryList : public RPCResponse {
  public:

    class MemoryListItem {
      public:
        MemoryListItem(unsigned int offset, unsigned int size, int semaphore);
        unsigned int get_offset ();
        unsigned int get_size ();
        int get_semaphore ();

      private:
        const unsigned int offset;
        const unsigned int size;
        const int semaphore;
    };

    MemoryList(Json::Value& result);
    std::vector<std::shared_ptr<MemoryListItem>>& get_items();

  private:
    std::vector<std::shared_ptr<MemoryListItem>> items;

};

using MemoryListPtr = std::shared_ptr<MemoryList>;
}
}
}

#endif
