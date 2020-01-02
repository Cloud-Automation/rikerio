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
        MemoryListItem(unsigned int offset, unsigned int size, int semaphore):
            offset(offset),
            size(size),
            semaphore(semaphore) {}

        unsigned int get_offset () {
            return offset;
        }

        unsigned int get_size () {
            return size;
        }

        int get_semaphore () {
            return semaphore;
        }

      private:
        const unsigned int offset;
        const unsigned int size;
        const int semaphore;
    };

    MemoryList(Json::Value& result) : RPCResponse(result) {

        if (!ok()) {
            return;
        }

        for (auto a : result["data"]) {
            std::shared_ptr<MemoryListItem> item = std::make_shared<MemoryListItem>(
                    a["offset"].asUInt(),
                    a["size"].asUInt(),
                    a["semaphore"].asInt());
            items.push_back(item);
        }

    }

    std::vector<std::shared_ptr<MemoryListItem>>& get_items() {
        return items;
    }

  private:
    std::vector<std::shared_ptr<MemoryListItem>> items;

};

using MemoryListPtr = std::shared_ptr<MemoryList>;
}
}
}

#endif
