#ifndef __RIKERIO_CLIENT_H__
#define __RIKERIO_CLIENT_H__

#include "json/json.h"
#include "jsonrpccpp/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"

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

class Client {
  public:

    /**
     * @params profile
     */
    Client(const std::string&);

    Response::v1::ConfigGetPtr config_get(Request::v1::ConfigGet&);

    Response::v1::MemoryAllocPtr memory_alloc(Request::v1::MemoryAlloc&);
    Response::v1::MemoryDeallocPtr memory_dealloc(Request::v1::MemoryDealloc&);
    Response::v1::MemoryListPtr memory_list(Request::v1::MemoryList&);

    Response::v1::DataAddPtr data_add(Request::v1::DataAdd&);
    Response::v1::DataRemovePtr data_remove(Request::v1::DataRemove&);
    Response::v1::DataListPtr data_list(Request::v1::DataList&);

    Response::v1::LinkAddPtr link_add(Request::v1::LinkAdd&);
    Response::v1::LinkRemovePtr link_remove(Request::v1::LinkRemove&);
    Response::v1::LinkListPtr link_list(Request::v1::LinkList&);

  protected:

    /* for testing only */
    Client(jsonrpc::IClientConnector&);

  private:
    const std::string socketFile;
    std::shared_ptr<jsonrpc::UnixDomainSocketClient> socketClient;
    jsonrpc::Client rpcClient;

};

}

#endif
