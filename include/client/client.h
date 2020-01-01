
#ifndef __JSONRPC_CPP_CLIENT_H__
#define __JSONRPC_CPP_CLIENT_H__
#include "json/json.h"
#include "jsonrpccpp/client.h"
#include "client/abstract-client.h"

namespace RikerIO {

class Client : public AbstractClient, public jsonrpc::Client {
  public:
    Client(jsonrpc::IClientConnector&);

    void config_get(AbstractClient::ConfigGetResponse&);

    void memory_alloc(unsigned int size, AbstractClient::MemoryAllocResponse&);
    void memory_dealloc(const std::string& token);
    void memory_list(AbstractClient::MemoryListResponse&);

    void data_create(const std::string& token, const std::string& id, DataCreateRequest req);
    void data_remove(const std::string& token, const std::string& pattern, unsigned int& resCount);
    void data_list(const std::string& pattern, DataListResponse&);

    void link_add(const std::string& key, std::vector<std::string>& list, unsigned int&);
    void link_list(const std::string& pattern, LinkListResponse&);

#if 0


    Json::Value data_get(const std::string& id, const std::string& token);
    Json::Value link_remove(const std::string& dataId, const std::string& linkId);
    Json::Value link_get(const std::string& id);
    Json::Value link_updates(const std::string& token);
#endif

};

}

#endif //JSONRPC_CPP_STUB_STUBCLIENT_H_
