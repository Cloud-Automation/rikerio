
#ifndef JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_
#define JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_

#include <jsonrpccpp/server.h>

class AbstractStubServer : public jsonrpc::AbstractServer<AbstractStubServer> {
  public:
    AbstractStubServer(jsonrpc::AbstractServerConnector &conn, jsonrpc::serverVersion_t type = jsonrpc::JSONRPC_SERVER_V2) : jsonrpc::AbstractServer<AbstractStubServer>(conn, type) {
        this->bindAndAddMethod(jsonrpc::Procedure("task.register", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "name", jsonrpc::JSON_STRING, "pid", jsonrpc::JSON_INTEGER, "track", jsonrpc::JSON_BOOLEAN, NULL), &AbstractStubServer::task_registerI);
        this->bindAndAddMethod(jsonrpc::Procedure("task.unregister", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "token", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::task_unregisterI);
        this->bindAndAddMethod(jsonrpc::Procedure("task.list", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,  NULL), &AbstractStubServer::task_listI);
        this->bindAndAddMethod(jsonrpc::Procedure("memory.alloc", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "size", jsonrpc::JSON_INTEGER, "token", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::memory_allocI);
        this->bindAndAddMethod(jsonrpc::Procedure("memory.dealloc", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "offset", jsonrpc::JSON_INTEGER, "token", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::memory_deallocI);
        this->bindAndAddMethod(jsonrpc::Procedure("memory.inspect", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, NULL), &AbstractStubServer::memory_inspectI);
        this->bindAndAddMethod(jsonrpc::Procedure("data.create", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "data", jsonrpc::JSON_OBJECT, "id", jsonrpc::JSON_STRING, "token", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::data_createI);
        this->bindAndAddMethod(jsonrpc::Procedure("data.remove", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "id", jsonrpc::JSON_STRING, "token", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::data_removeI);
        this->bindAndAddMethod(jsonrpc::Procedure("data.list", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "id", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::data_listI);
        this->bindAndAddMethod(jsonrpc::Procedure("data.get", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "id", jsonrpc::JSON_STRING, "token", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::data_getI);
        this->bindAndAddMethod(jsonrpc::Procedure("link.add", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "dataId", jsonrpc::JSON_STRING, "linkId", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::link_addI);
        this->bindAndAddMethod(jsonrpc::Procedure("link.remove", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "dataId", jsonrpc::JSON_STRING, "linkId", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::link_removeI);
        this->bindAndAddMethod(jsonrpc::Procedure("link.list", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "pattern", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::link_listI);
        this->bindAndAddMethod(jsonrpc::Procedure("link.get", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "id", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::link_getI);
        this->bindAndAddMethod(jsonrpc::Procedure("link.updates", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, "token", jsonrpc::JSON_STRING, NULL), &AbstractStubServer::link_updatesI);
    }

    inline virtual void task_registerI(const Json::Value &request, Json::Value &response) {
        response = this->task_register(request["name"].asString(), request["pid"].asInt(), request["track"].asBool());
    }
    inline virtual void task_unregisterI(const Json::Value &request, Json::Value &response) {
        response = this->task_unregister(request["token"].asString());
    }
    inline virtual void task_listI(const Json::Value &/*request*/, Json::Value &response) {
        response = this->task_list();
    }
    inline virtual void memory_allocI(const Json::Value &request, Json::Value &response) {
        response = this->memory_alloc(request["size"].asInt(), request["token"].asString());
    }
    inline virtual void memory_deallocI(const Json::Value &request, Json::Value &response) {
        response = this->memory_dealloc(request["offset"].asInt(), request["token"].asString());
    }
    inline virtual void memory_inspectI(const Json::Value &/*request*/, Json::Value &response) {
        response = this->memory_inspect();
    }
    inline virtual void data_createI(const Json::Value &request, Json::Value &response) {
        response = this->data_create(request["data"], request["id"].asString(), request["token"].asString());
    }
    inline virtual void data_removeI(const Json::Value &request, Json::Value &response) {
        response = this->data_remove(request["id"].asString(), request["token"].asString());
    }
    inline virtual void data_listI(const Json::Value &request, Json::Value &response) {
        response = this->data_list(request["id"].asString());
    }
    inline virtual void data_getI(const Json::Value &request, Json::Value &response) {
        response = this->data_get(request["id"].asString(), request["id"].asString());
    }
    inline virtual void link_addI(const Json::Value &request, Json::Value &response) {
        response = this->link_add(request["dataId"].asString(), request["linkId"].asString());
    }
    inline virtual void link_removeI(const Json::Value &request, Json::Value &response) {
        response = this->link_remove(request["dataId"].asString(), request["linkId"].asString());
    }
    inline virtual void link_listI(const Json::Value &request, Json::Value &response) {
        response = this->link_list(request["pattern"].asString());
    }
    inline virtual void link_getI(const Json::Value &request, Json::Value &response) {
        response = this->link_get(request["id"].asString());
    }
    inline virtual void link_updatesI(const Json::Value &request, Json::Value &response) {
        response = this->link_updates(request["token"].asString());
    }

    virtual Json::Value task_register(const std::string& name, int pid, bool track) = 0;
    virtual Json::Value task_unregister(const std::string& token) = 0;
    virtual Json::Value task_list() = 0;
    virtual Json::Value memory_alloc(int size, const std::string& token) = 0;
    virtual Json::Value memory_dealloc(int offset, const std::string& token) = 0;
    virtual Json::Value memory_inspect() = 0;
    virtual Json::Value data_create(const Json::Value& data, const std::string& id, const std::string& token) = 0;
    virtual Json::Value data_remove(const std::string& id, const std::string& token) = 0;
    virtual Json::Value data_list(const std::string& id) = 0;
    virtual Json::Value data_get(const std::string& id) = 0;
    virtual Json::Value link_add(const std::string& dataId, const std::string& linkId) = 0;
    virtual Json::Value link_remove(const std::string& dataId, const std::string& linkId) = 0;
    virtual Json::Value link_list(const std::string& pattern) = 0;
    virtual Json::Value link_get(const std::string& id) = 0;
    virtual Json::Value link_updates(const std::string& token) = 0;
};

#endif //JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_
