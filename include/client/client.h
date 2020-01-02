#ifndef __RIKERIO_CLIENT_H__
#define __RIKERIO_CLIENT_H__

#include "json/json.h"
#include "jsonrpccpp/client.h"
#include "client/abstract-client.h"

#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"

namespace RikerIO {

class Client : public AbstractClient {
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

#endif //JSONRPC_CPP_STUB_STUBCLIENT_H_
