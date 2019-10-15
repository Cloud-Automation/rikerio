#include "client/client.h"

using namespace RikerIO;

Client::Client(jsonrpc::IClientConnector &conn) :
    jsonrpc::Client(conn, jsonrpc::JSONRPC_CLIENT_V2) { }

std::string Client::task_register(const std::string& name, int pid, bool track) {
    Json::Value p;
    p["name"] = name;
    p["pid"] = pid;
    p["track"] = track;
    Json::Value result = this->CallMethod("task.register", p);

    if (!result.isObject()) {
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
    }

    unsigned int resCode = result["code"].asUInt();
    std::string resToken = result["data"].asString();

    return resToken;
}

void Client::task_unregister(const std::string& token) {
    Json::Value p;
    p["token"] = token;
    Json::Value result = this->CallMethod("task.unregister", p);
    if (result.isObject())
        return;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

void Client::task_list(TaskList& list) {
    Json::Value p;
    p = Json::nullValue;
    Json::Value result = this->CallMethod("task.list", p);
    if (result.isObject())
        return;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::memory_alloc(int size, const std::string& token) {
    Json::Value p;
    p["size"] = size;
    p["token"] = token;
    Json::Value result = this->CallMethod("memory.alloc", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::memory_dealloc(int offset, const std::string& token) {
    Json::Value p;
    p["offset"] = offset;
    p["token"] = token;
    Json::Value result = this->CallMethod("memory_dealloc", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::memory_inspect() {
    Json::Value p;
    p = Json::nullValue;
    Json::Value result = this->CallMethod("memory.inspect", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::data_create(const Json::Value& data, const std::string& id, const std::string& token) {
    Json::Value p;
    p["data"] = data;
    p["id"] = id;
    p["token"] = token;
    Json::Value result = this->CallMethod("data.create", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::data_remove(const std::string& id, const std::string& token) {
    Json::Value p;
    p["id"] = id;
    p["token"] = token;
    Json::Value result = this->CallMethod("data.remove", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::data_list(const std::string& id) {
    Json::Value p;
    p["id"] = id;
    Json::Value result = this->CallMethod("data.list", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::data_get(const std::string& id, const std::string& token) {
    Json::Value p;
    p["id"] = id;
    p["token"] = token;
    Json::Value result = this->CallMethod("data.get", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::link_add(const std::string& dataId, const std::string& linkId) {
    Json::Value p;
    p["dataId"] = dataId;
    p["linkId"] = linkId;
    Json::Value result = this->CallMethod("link.add", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::link_remove(const std::string& dataId, const std::string& linkId) {
    Json::Value p;
    p["dataId"] = dataId;
    p["linkId"] = linkId;
    Json::Value result = this->CallMethod("link.remove", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::link_list(const std::string& pattern) {
    Json::Value p;
    p["pattern"] = pattern;
    Json::Value result = this->CallMethod("link.list", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::link_get(const std::string& id) {
    Json::Value p;
    p["id"] = id;
    Json::Value result = this->CallMethod("link.get", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

Json::Value Client::link_updates(const std::string& token) {
    Json::Value p;
    p["token"] = token;
    Json::Value result = this->CallMethod("link.updates", p);
    if (result.isObject())
        return result;
    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}
