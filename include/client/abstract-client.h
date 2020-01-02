
#ifndef ABSTRACT_STUB_STUBCLIENT_H_
#define ABSTRACT_STUB_STUBCLIENT_H_

#include <set>
#include <vector>
#include <memory>
#include <string>

#include "client/response.h"
#include "client/response/config-get.h"
#include "client/response/memory-alloc.h"
#include "client/response/memory-dealloc.h"
#include "client/response/memory-list.h"
#include "client/response/data-add.h"
#include "client/response/data-remove.h"
#include "client/response/data-list.h"
#include "client/response/link-add.h"
#include "client/response/link-remove.h"
#include "client/response/link-list.h"

#include "client/request.h"
#include "client/request/config-get.h"
#include "client/request/memory-alloc.h"
#include "client/request/memory-dealloc.h"
#include "client/request/memory-list.h"
#include "client/request/data-add.h"
#include "client/request/data-remove.h"
#include "client/request/data-list.h"
#include "client/request/link-add.h"
#include "client/request/link-remove.h"
#include "client/request/link-list.h"

namespace RikerIO {

class AbstractClient {
  public:

    virtual ~AbstractClient() { };

    virtual Response::v1::ConfigGetPtr config_get(Request::v1::ConfigGet&) = 0;
    virtual Response::v1::MemoryAllocPtr memory_alloc(Request::v1::MemoryAlloc&) = 0;
    virtual Response::v1::MemoryDeallocPtr memory_dealloc(Request::v1::MemoryDealloc&) = 0;
    virtual Response::v1::MemoryListPtr memory_list(Request::v1::MemoryList&) = 0;

    virtual Response::v1::DataAddPtr data_add(Request::v1::DataAdd&) = 0;
    virtual Response::v1::DataRemovePtr data_remove(Request::v1::DataRemove&)  = 0;
    virtual Response::v1::DataListPtr data_list(Request::v1::DataList&) = 0;

    virtual Response::v1::LinkAddPtr link_add(Request::v1::LinkAdd&) = 0;
    virtual Response::v1::LinkRemovePtr link_remove(Request::v1::LinkRemove&) = 0;
    virtual Response::v1::LinkListPtr link_list(Request::v1::LinkList&) = 0;
};

}

#endif //ABSTRACT_STUB_STUBCLIENT_H_
