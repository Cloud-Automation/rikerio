#ifndef __RIKERIO_MEMORY_LIST_RESPONSE_H__
#define __RIKERIO_MEMORY_LIST_RESPONSE_H__

#include "common/semaphore.h"
#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class MemoryList : public RPCResponse {
  public:

    class MemoryListItem {
      public:
        MemoryListItem(unsigned int offset, unsigned int size, int semaphore, uint8_t* memory_ptr);
        MemoryListItem(unsigned int offset, unsigned int size, int semaphore);
        unsigned int get_offset ();
        unsigned int get_size ();

        std::shared_ptr<Semaphore> get_semaphore();

      private:
        const unsigned int offset;
        const unsigned int size;
        std::shared_ptr<Semaphore> semaphore;

        uint8_t* alloc_ptr;
    };

    MemoryList(Json::Value& result, uint8_t* memory_ptr);
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
