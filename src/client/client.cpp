#include "client/client.h"
#include "common/config.h"

using namespace RikerIO;

Client::Client(const std::string& profile) :
    socketFile(Config::CreateSocketPath(profile)),
    socketClient(std::make_shared<jsonrpc::UnixDomainSocketClient>(socketFile)),
    rpcClient(*socketClient, jsonrpc::JSONRPC_CLIENT_V2) { }

Client::Client(jsonrpc::IClientConnector &conn) :
    socketFile(""),
    socketClient(nullptr),
    rpcClient(conn, jsonrpc::JSONRPC_CLIENT_V2) { }

Response::v1::ConfigGetPtr Client::config_get(Request::v1::ConfigGet& req) {

    Json::Value p = req.create_params();
    Json::Value result = rpcClient.CallMethod("v1/configGet", p);

    return std::make_shared<Response::v1::ConfigGet>(result);

}

Response::v1::MemoryAllocPtr Client::memory_alloc(Request::v1::MemoryAlloc& req) {

    Json::Value p = req.create_params();
    Json::Value result = rpcClient.CallMethod("v1/memoryAlloc", p);

    return std::make_shared<Response::v1::MemoryAlloc>(result);

}


Response::v1::MemoryDeallocPtr Client::memory_dealloc(Request::v1::MemoryDealloc& req) {

    Json::Value p = req.create_params();
    Json::Value result = rpcClient.CallMethod("v1/memoryDealloc", p);

    return std::make_shared<Response::v1::MemoryDealloc>(result);

}


Response::v1::MemoryListPtr Client::memory_list(Request::v1::MemoryList& req) {

    Json::Value p = req.create_params();
    Json::Value result = rpcClient.CallMethod("v1/memoryList", p);

    return std::make_shared<Response::v1::MemoryList>(result);

}


Response::v1::DataAddPtr Client::data_add(Request::v1::DataAdd& req) {

    Json::Value p = req.create_params();
    Json::Value result = rpcClient.CallMethod("v1/dataCreate", p);

    return std::make_shared<Response::v1::DataAdd>(result);

}

Response::v1::DataRemovePtr Client::data_remove(Request::v1::DataRemove& req) {

    Json::Value p = req.create_params();
    Json::Value result = rpcClient.CallMethod("v1/dataRemove", p);

    return std::make_shared<Response::v1::DataRemove>(result);

}


Response::v1::DataListPtr Client::data_list(Request::v1::DataList& req) {

    Json::Value p = req.create_params();
    Json::Value result = rpcClient.CallMethod("v1/dataList", p);

    return std::make_shared<Response::v1::DataList>(result);

}


Response::v1::LinkAddPtr Client::link_add(Request::v1::LinkAdd& req) {

    Json::Value p = req.create_params();
    Json::Value result = rpcClient.CallMethod("v1/linkAdd", p);

    return std::make_shared<Response::v1::LinkAdd>(result);

}

Response::v1::LinkRemovePtr Client::link_remove(Request::v1::LinkRemove& req) {

    Json::Value p = req.create_params();
    Json::Value result = rpcClient.CallMethod("v1/linkRemove", p);

    return std::make_shared<Response::v1::LinkRemove>(result);

}


Response::v1::LinkListPtr Client::link_list(Request::v1::LinkList& req) {

    Json::Value p = req.create_params();
    Json::Value result = rpcClient.CallMethod("v1/linkList", p);

    return std::make_shared<Response::v1::LinkList>(result);

}
