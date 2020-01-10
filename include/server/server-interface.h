#ifndef __RIKERIO_SERVER_INTERFACE_H__
#define __RIKERIO_SERVER_INTERFACE_H__

#include "string"
#include "vector"
#include "common/type.h"
#include "common/mem-position.h"
#include "server/memory-area.h"

#include "server/config-get.h"
#include "server/memory-alloc.h"
#include "server/memory-dealloc.h"
#include "server/memory-list.h"
#include "server/memory-get.h"
#include "server/data-add.h"
#include "server/data-remove.h"
#include "server/data-list.h"
#include "server/link-add.h"
#include "server/link-remove.h"
#include "server/link-list.h"

namespace RikerIO {

class ServerInterface {

  public:

    class ServerError : public std::exception {
      public:
        ServerError(int code, const std::string msg) : code(code), msg(msg) {
        }
        int getCode() {
            return code;
        }
        const std::string& getMsg() {
            return msg;
        }
      private:
        int code;
        const std::string msg;
    };



    virtual void config_get(ConfigResponse&) = 0;
    virtual void memory_alloc(MemoryAllocRequest&, MemoryAllocResponse&) = 0;
    virtual void memory_dealloc(MemoryDeallocRequest&) = 0;
    virtual void memory_list(MemoryListResponse&) = 0;
    virtual void memory_get(MemoryGetRequest&, MemoryGetResponse&) = 0;
    virtual void data_add(DataAddRequest&, DataAddResponse&) = 0;
    virtual void data_remove(DataRemoveRequest&, DataRemoveResponse& res) = 0;
    virtual void data_list(DataListRequest&, DataListResponse&) = 0;
    virtual void link_add(LinkAddRequest&, LinkAddResponse&) = 0;
    virtual void link_remove(LinkRemoveRequest&, LinkRemoveResponse&) = 0;
    virtual void link_list(LinkListRequest&, LinkListResponse&) = 0;


};


}

#endif
