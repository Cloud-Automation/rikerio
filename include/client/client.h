
#ifndef __JSONRPC_CPP_CLIENT_H__
#define __JSONRPC_CPP_CLIENT_H__

#include "json/json.h"
#include "jsonrpccpp/client.h"
#include "client/abstract-client.h"

namespace RikerIO {

class Client : public AbstractClient, public jsonrpc::Client {
  public:
    Client(jsonrpc::IClientConnector);

    std::string task_register(const std::string& name, int pid, bool track);
    void        task_unregister(const std::string& token);
    void        task_list(TaskList&);
    Json::Value memory_alloc(int size, const std::string& token);
    Json::Value memory_dealloc(int offset, const std::string& token);
    Json::Value memory_inspect();
    Json::Value data_create(const Json::Value& data, const std::string& id, const std::string& token);
    Json::Value data_remove(const std::string& id, const std::string& token);
    Json::Value data_list(const std::string& id);
    Json::Value data_get(const std::string& id, const std::string& token);
    Json::Value link_add(const std::string& dataId, const std::string& linkId);
    Json::Value link_remove(const std::string& dataId, const std::string& linkId);
    Json::Value link_list(const std::string& pattern);
    Json::Value link_get(const std::string& id);
    Json::Value link_updates(const std::string& token);

};

}

#endif //JSONRPC_CPP_STUB_STUBCLIENT_H_
