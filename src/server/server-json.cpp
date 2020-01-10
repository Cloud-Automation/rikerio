#include "server/server-json.h"

JsonServer::JsonServer(
    jsonrpc::AbstractServerConnector &conn,
    RikerIO::ServerInterface& server,
    jsonrpc::serverVersion_t type) :
    jsonrpc::AbstractServer<JsonServer>(conn, type),
    server(server) {

    this->bindAndAddMethod(
        jsonrpc::Procedure("v1/configGet", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, NULL),
        &JsonServer::configI);

    this->bindAndAddMethod(
        jsonrpc::Procedure("v1/memoryAlloc", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                           "size", jsonrpc::JSON_INTEGER, NULL),
        &JsonServer::memory_allocI);

    this->bindAndAddMethod(
        jsonrpc::Procedure("v1/memoryDealloc", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                           "token", jsonrpc::JSON_STRING, NULL),
        &JsonServer::memory_deallocI);

    this->bindAndAddMethod(
        jsonrpc::Procedure("v1/memoryList", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT, NULL),
        &JsonServer::memory_listI);

    this->bindAndAddMethod(
        jsonrpc::Procedure("v1/memoryGet", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                           "offset", jsonrpc::JSON_INTEGER, NULL),
        &JsonServer::memory_getI);

    this->bindAndAddMethod(
        jsonrpc::Procedure("v1/dataCreate", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                           "data", jsonrpc::JSON_OBJECT, NULL),
        &JsonServer::data_addI);

    this->bindAndAddMethod(
        jsonrpc::Procedure("v1/dataRemove", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                           "pattern", jsonrpc::JSON_STRING, NULL),
        &JsonServer::data_removeI);

    this->bindAndAddMethod(
        jsonrpc::Procedure("v1/dataList", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                           "pattern", jsonrpc::JSON_STRING, NULL),
        &JsonServer::data_listI);

    this->bindAndAddMethod(
        jsonrpc::Procedure("v1/linkAdd", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                           "key", jsonrpc::JSON_STRING, NULL),
        &JsonServer::link_addI);

    this->bindAndAddMethod(
        jsonrpc::Procedure("v1/linkRemove", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                           "pattern", jsonrpc::JSON_STRING, NULL),
        &JsonServer::link_removeI);

    this->bindAndAddMethod(
        jsonrpc::Procedure("v1/linkList", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                           "pattern", jsonrpc::JSON_STRING, NULL),
        &JsonServer::link_listI);

}


inline void JsonServer::configI(const Json::Value& request, Json::Value& response) {

    spdlog::debug("v1/configGet Request : {}", request.toStyledString());

    try {

        RikerIO::ConfigResponse res;
        server.config_get(res);

        response["code"] = RikerIO::Error::NO_ERROR;
        response["data"] = Json::objectValue;
        response["data"]["id"] = res.profile;
        response["data"]["version"] = res.version;
        response["data"]["shm"] = res.shmFile;
        response["data"]["size"] = res.size;
        response["data"]["cycle"] = res.defaultCycle;

    }  catch (RikerIO::ServerInterface::ServerError& e) {

        response["code"] = e.getCode();
        response["message"] = e.getMsg();

    }

    spdlog::debug("Response {}", response.toStyledString());


}

inline void JsonServer::memory_allocI(const Json::Value &request, Json::Value &response) {

    spdlog::debug("v1/memoryAlloc Request : {}", request.toStyledString());

    try {

        RikerIO::MemoryAllocRequest req(request["size"].asUInt());
        RikerIO::MemoryAllocResponse res;

        server.memory_alloc(req, res);

        response["code"] = RikerIO::Error::NO_ERROR;
        response["data"] = Json::objectValue;
        response["data"]["offset"] = res.offset;
        response["data"]["token"] = res.token;
        response["data"]["semaphore"] = res.semaphore;

    } catch (RikerIO::ServerInterface::ServerError& e) {

        response["code"] = e.getCode();
        response["message"] = e.getMsg();

    }

    spdlog::debug("Response {}", response.toStyledString());

}

inline void JsonServer::memory_deallocI(const Json::Value &request, Json::Value &response) {

    spdlog::debug("v1/memoryDealloc Request : {}", request.toStyledString());

    try {

        RikerIO::MemoryDeallocRequest req(request["token"].asString());

        server.memory_dealloc(req);

        response["code"] = RikerIO::Error::NO_ERROR;

    } catch (RikerIO::ServerInterface::ServerError& e) {

        response["code"] = e.getCode();
        response["message"] = e.getMsg();

    }

    spdlog::debug("Response {}", response.toStyledString());

}

inline void JsonServer::memory_listI(const Json::Value & request, Json::Value &response) {

    spdlog::debug("v1/memoryList Request : {}", request.toStyledString());

    RikerIO::MemoryListResponse res;

    server.memory_list(res);

    response["code"] = 0;
    response["data"] = Json::arrayValue;

    for (auto ma : res.data) {

        Json::Value item;
        item["offset"] = ma->get_offset();
        item["size"] = ma->get_size();
        item["semaphore"] = ma->get_semaphore();

        response["data"].append(item);

    }

    spdlog::debug("Response {}", response.toStyledString());

}

inline void JsonServer::memory_getI(const Json::Value& request, Json::Value &response) {

    spdlog::debug("v1/memoryGet Request : {}", request.toStyledString());

    try {

        RikerIO::MemoryGetRequest req(request["offset"].asUInt());
        RikerIO::MemoryGetResponse res;

        server.memory_get(req, res);

        Json::Value item;
        item["offset"] = res.data->get_offset();
        item["size"] = res.data->get_size();
        item["semaphore"] = res.data->get_semaphore();

        response["code"] = RikerIO::NO_ERROR;
        response["data"] = item;

    } catch (RikerIO::ServerInterface::ServerError& e) {
        response["code"] = e.getCode();
        response["message"] = e.getMsg();
    }

    spdlog::debug("Response {}", response.toStyledString());

}

inline void JsonServer::data_addI(const Json::Value &request, Json::Value &response) {

    spdlog::debug("v1/dataAdd Request : {}", request.toStyledString());

    Json::Value data = request["data"];

    bool hasId = data["id"] && data["id"].isString();
    bool hasToken = data["token"] && data["token"].isString();
    bool hasType = data["type"] && data["type"].isString();
    bool hasOffset = data["offset"] && data["offset"].isString();

    if (!hasType || !hasOffset || !hasId) {
        response["code"] = RikerIO::Error::BAD_REQUEST;
        response["message"] = "Missing or incorrect id/type/offset.";
        spdlog::debug("Response {}", response.toStyledString());

        return;
    }

    try {

        std::string id = data["id"].asString();
        std::string token = hasToken ? data["token"].asString() : "";
        std::string type = data["type"].asString();
        std::string offset = data["offset"].asString();

        RikerIO::DataAddRequest req(id, token, type, offset);
        RikerIO::DataAddResponse res;

        server.data_add(req, res);
        response["code"] = RikerIO::NO_ERROR;
        response["data"] = Json::objectValue;
        response["data"]["id"] = id;
        response["data"]["type"] = res.type.to_string();
        response["data"]["offset"] = res.offset.to_string();
        response["data"]["semaphore"] = res.semaphore;

    } catch (RikerIO::Type::TypeError& e) {
        response["code"] = RikerIO::Error::BAD_REQUEST;
        response["message"] = e.getMessage();
    } catch (RikerIO::ServerInterface::ServerError& e) {
        response["code"] = e.getCode();
        response["message"] = e.getMsg();
    }

    spdlog::debug("Response {}", response.toStyledString());

}


inline void JsonServer::data_removeI(const Json::Value &request, Json::Value &response) {

    spdlog::debug("v1/dataRemove Request : {}", request.toStyledString());

    try {

        std::string token = (request["token"] && request["token"].isString()) ?
                            request["token"].asString() : "";

        RikerIO::DataRemoveRequest req(request["pattern"].asString(), token);
        RikerIO::DataRemoveResponse res;

        server.data_remove(req, res);

        response["code"] = RikerIO::NO_ERROR;
        response["data"] = Json::objectValue;
        response["data"]["count"] = res.count;

    } catch (RikerIO::ServerInterface::ServerError& e) {
        response["code"] = e.getCode();
        response["message"] = e.getMsg();
    }

    spdlog::debug("Response {}", response.toStyledString());

}


inline void JsonServer::data_listI(const Json::Value &request, Json::Value &response) {

    spdlog::debug("v1/dataList Request : {}", request.toStyledString());

    try {

        Json::Value resData = Json::arrayValue;

        RikerIO::DataListRequest req(request["pattern"].asString());
        RikerIO::DataListResponse res;

        server.data_list(req, res);

        Json::Value item;

        for (auto d : res.list) {

            item["id"] = d.id;
            item["offset"] = d.offset.to_string();
            item["type"] = d.type.to_string();
            item["semaphore"] = d.semaphore;
            item["private"] = d.is_private;

            resData.append(item);
        }

        response["code"] = 0;
        response["data"] = resData;

    } catch (RikerIO::ServerInterface::ServerError& e) {

        response["code"] = e.getCode();
        response["message"] = e.getMsg();

    }

    spdlog::debug("Response {}", response.toStyledString());

}


inline void JsonServer::link_addI(const Json::Value &request, Json::Value &response) {

    spdlog::debug("v1/linkAdd Request : {}", request.toStyledString());

    RikerIO::LinkAddRequest req(request["key"].asString());
    RikerIO::LinkAddResponse res;

    if (!request["data"] || !request["data"].isArray()) {
        response["code"] = RikerIO::BAD_REQUEST;
        response["message"] = "Data parameter musst be a list of strings.";
        return;
    }

    const Json::Value& data = request["data"];

    if (data.size() == 0) {
        response["code"] = RikerIO::BAD_REQUEST;
        response["message"] = "Data list cannot be empty.";
        return;
    }


    for (auto j : data) {

        if (!j.isString()) {
            response["code"] = RikerIO::BAD_REQUEST;
            response["message"] = "Data parameter musst be a list of strings.";
            return;
        }

        req.ids.push_back(j.asString());

    }


    server.link_add(req, res);

    response["code"] = RikerIO::NO_ERROR;
    response["data"] = Json::objectValue;
    response["data"]["counter"] = res.counter;

    spdlog::debug("Response {}", response.toStyledString());

}

inline void JsonServer::link_removeI(const Json::Value &request, Json::Value &response) {

    spdlog::debug("v1/linkRemove Request : {}", request.toStyledString());

    RikerIO::LinkRemoveRequest req(request["pattern"].asString());
    RikerIO::LinkRemoveResponse res;

    if (request["data"] && !request["data"].isArray()) {
        response["code"] = RikerIO::BAD_REQUEST;
        response["message"] = "Data parameter musst be a list of strings.";
        return;
    }

    if (request["data"] && request["data"].isArray()) {

        const Json::Value& data = request["data"];

        for (auto j : data) {

            if (!j.isString()) {
                response["code"] = RikerIO::BAD_REQUEST;
                response["message"] = "Data parameter musst be a list of strings.";
                return;
            }

            req.ids.push_back(j.asString());

        }
    }

    server.link_remove(req, res);

    response["code"] = RikerIO::NO_ERROR;
    response["data"] = Json::objectValue;
    response["data"]["counter"] = res.counter;

    spdlog::debug("Response {}", response.toStyledString());

}

inline void JsonServer::link_listI(const Json::Value &request, Json::Value &response) {

    spdlog::debug("v1/linkList Request : {}", request.toStyledString());

    RikerIO::LinkListRequest req(request["pattern"].asString());
    RikerIO::LinkListResponse res;

    server.link_list(req, res);

    response["pattern"] = request["pattern"].asString();
    response["data"] = Json::arrayValue;

    for (auto i : res.list) {

        Json::Value item;
        Json::Value data;

        if (i.has_data) {

            data["offset"] = i.data_item.offset.to_string();
            data["type"] = i.data_item.type.to_string();
            data["semaphore"] = i.data_item.semaphore;
            data["private"] = i.data_item.is_private;
            item["data"] = data;

        }

        item["key"] = i.key;
        item["id"] = i.id;

        response["data"].append(item);

    }

    spdlog::debug("Response {}", response.toStyledString());

}
