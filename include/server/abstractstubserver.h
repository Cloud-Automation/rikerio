
#ifndef JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_
#define JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_

#include "spdlog/spdlog.h"
#include "jsonrpccpp/server.h"
#include "algorithm"
#include "common/error.h"
#include "common/type.h"
#include "common/mem-position.h"
#include "server/data.h"
#include "server/memory.h"

class AbstractStubServer : public jsonrpc::AbstractServer<AbstractStubServer> {
  public:
    AbstractStubServer(
        jsonrpc::AbstractServerConnector &conn,
        jsonrpc::serverVersion_t type = jsonrpc::JSONRPC_SERVER_V2) :
        jsonrpc::AbstractServer<AbstractStubServer>(conn, type) {

        this->bindAndAddMethod(
            jsonrpc::Procedure(
                "v1/configGet",
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT, NULL),
            &AbstractStubServer::configI);

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/memoryAlloc", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_OBJECT,
                               "size", jsonrpc::JSON_INTEGER, NULL),
            &AbstractStubServer::memory_allocI);

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/memoryDealloc", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_OBJECT,
                               "token", jsonrpc::JSON_STRING, NULL),
            &AbstractStubServer::memory_deallocI);

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/memoryList", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_OBJECT, NULL),
            &AbstractStubServer::memory_listI);

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/memoryGet", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_OBJECT,
                               "offset", jsonrpc::JSON_INTEGER, NULL),
            &AbstractStubServer::memory_getI);

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/dataCreate", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_OBJECT,
                               "data", jsonrpc::JSON_OBJECT, NULL),
            &AbstractStubServer::data_addI);

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/dataRemove", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_OBJECT,
                               "pattern", jsonrpc::JSON_STRING, NULL),
            &AbstractStubServer::data_removeI);

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/dataList", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_OBJECT,
                               "pattern", jsonrpc::JSON_STRING, NULL),
            &AbstractStubServer::data_listI);

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/linkAdd", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_OBJECT, "key", jsonrpc::JSON_STRING, NULL),
            &AbstractStubServer::link_addI);

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/linkRemove", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_OBJECT, "pattern",
                               jsonrpc::JSON_STRING, NULL),
            &AbstractStubServer::link_removeI);

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/linkList", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_OBJECT, "pattern",
                               jsonrpc::JSON_STRING, NULL),
            &AbstractStubServer::link_listI);

    }


    inline virtual void configI(const Json::Value& request, Json::Value& response) {

        spdlog::debug("Request : {}", request.toStyledString());

        try {

            ConfigResponse res;
            this->get_config(res);

            response["code"] = RikerIO::Error::NO_ERROR;
            response["data"] = Json::objectValue;
            response["data"]["id"] = res.profile;
            response["data"]["version"] = res.version;
            response["data"]["shm"] = res.shmFile;
            response["data"]["size"] = res.size;
            response["data"]["cycle"] = res.defaultCycle;

        }  catch (ServerError& e) {

            response["code"] = e.getCode();
            response["message"] = e.getMsg();

        }

        spdlog::debug("Response {}", response.toStyledString());


    }

    inline virtual void memory_allocI(const Json::Value &request, Json::Value &response) {

        spdlog::debug("Request : {}", request.toStyledString());

        try {

            MemoryAllocResponse res;

            this->memory_alloc(request["size"].asInt(), res);

            response["code"] = RikerIO::Error::NO_ERROR;
            response["data"] = Json::objectValue;
            response["data"]["offset"] = res.offset;
            response["data"]["token"] = res.token;
            response["data"]["semaphore"] = res.semaphore;

        } catch (ServerError& e) {

            response["code"] = e.getCode();
            response["message"] = e.getMsg();

        }

        spdlog::debug("Response {}", response.toStyledString());

    }

    inline virtual void memory_deallocI(const Json::Value &request, Json::Value &response) {

        spdlog::debug("Request : {}", request.toStyledString());

        try {

            this->memory_dealloc(request["token"].asString());

            response["code"] = RikerIO::Error::NO_ERROR;

        } catch (ServerError& e) {

            response["code"] = e.getCode();
            response["message"] = e.getMsg();

        }

        spdlog::debug("Response {}", response.toStyledString());

    }

    inline virtual void memory_listI(const Json::Value & request, Json::Value &response) {

        spdlog::debug("Request : {}", request.toStyledString());

        MemoryListResponse res;

        memory_list(res);

        response["code"] = 0;
        response["data"] = Json::arrayValue;

        for (auto ma : res.data) {

            Json::Value item;
            item["offset"] = ma->getOffset();
            item["size"] = ma->getSize();
            item["semaphore"] = ma->getSemaphore();

            response["data"].append(item);

        }

        spdlog::debug("Response {}", response.toStyledString());

    }

    inline virtual void memory_getI(const Json::Value& request, Json::Value &response) {

        spdlog::debug("Request : {}", request.toStyledString());

        try {

            MemoryGetResponse res;

            memory_get(request["offset"].asInt(), res);

            Json::Value item;
            item["offset"] = res.data->getOffset();
            item["size"] = res.data->getSize();
            item["semaphore"] = res.data->getSemaphore();

            response["code"] = RikerIO::NO_ERROR;
            response["data"] = item;

        } catch (ServerError& e) {
            response["code"] = e.getCode();
            response["message"] = e.getMsg();
        }

        spdlog::debug("Response {}", response.toStyledString());

    }

    inline virtual void data_addI(const Json::Value &request, Json::Value &response) {

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
            RikerIO::Type type(data["type"].asString());
            RikerIO::MemoryPosition offset(data["offset"].asString());

            DataAddRequest req = { token, type, offset };
            DataAddResponse res = { RikerIO::Type("1bit"), RikerIO::MemoryPosition(0), 0 };

            data_add(id, req, res);
            response["code"] = RikerIO::NO_ERROR;
            response["data"] = Json::objectValue;
            response["data"]["id"] = id;
            response["data"]["type"] = res.type.to_string();
            response["data"]["offset"] = res.offset.to_string();
            response["data"]["semaphore"] = res.semaphore;

        } catch (RikerIO::Type::TypeError& e) {
            response["code"] = RikerIO::Error::BAD_REQUEST;
            response["message"] = e.getMessage();
        } catch (ServerError& e) {
            response["code"] = e.getCode();
            response["message"] = e.getMsg();
        }

        spdlog::debug("Response {}", response.toStyledString());

    }


    inline virtual void data_removeI(const Json::Value &request, Json::Value &response) {

        spdlog::debug("Request : {}", request.toStyledString());

        try {

            std::string token = (request["token"] && request["token"].isString()) ?
                                request["token"].asString() : "";

            DataRemoveResponse res;

            data_remove(request["pattern"].asString(), token, res);

            response["code"] = RikerIO::NO_ERROR;
            response["data"] = Json::objectValue;
            response["data"]["count"] = res.count;

        } catch (ServerError& e) {
            response["code"] = e.getCode();
            response["message"] = e.getMsg();
        }

        spdlog::debug("Response {}", response.toStyledString());

    }


    inline virtual void data_listI(const Json::Value &request, Json::Value &response) {

        spdlog::debug("Request : {}", request.toStyledString());

        try {

            Json::Value resData = Json::arrayValue;

            DataListResponse res;
            data_list(request["pattern"].asString(), res);

            Json::Value item;

            for (auto d : res.list) {

                item["id"] = d.id;
                item["offset"] = d.offset.to_string();
                item["type"] = d.type.to_string();
                item["semaphore"] = d.semaphore;
                item["private"] = d.isPrivate;

                resData.append(item);
            }

            response["code"] = 0;
            response["data"] = resData;

        } catch (ServerError& e) {

            response["code"] = e.getCode();
            response["message"] = e.getMsg();

        }

        spdlog::debug("Response {}", response.toStyledString());

    }


    inline virtual void link_addI(const Json::Value &request, Json::Value &response) {

        spdlog::debug("Request : {}", request.toStyledString());

        std::vector<std::string> dataIds;

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

            dataIds.push_back(j.asString());

        }

        unsigned int counter = 0;

        this->link_add(request["key"].asString(), dataIds, counter);

        response["code"] = RikerIO::NO_ERROR;
        response["data"] = Json::objectValue;
        response["data"]["counter"] = counter;

        spdlog::debug("Response {}", response.toStyledString());

    }

    inline virtual void link_removeI(const Json::Value &request, Json::Value &response) {

        spdlog::debug("Request : {}", request.toStyledString());

        std::vector<std::string> dataIds;

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

                dataIds.push_back(j.asString());

            }
        }
        unsigned int counter = 0;

        this->link_remove(request["pattern"].asString(), dataIds, counter);

        response["code"] = RikerIO::NO_ERROR;
        response["data"] = Json::objectValue;
        response["data"]["counter"] = counter;

        spdlog::debug("Response {}", response.toStyledString());

    }

    inline virtual void link_listI(const Json::Value &request, Json::Value &response) {

        spdlog::debug("Request : {}", request.toStyledString());

        LinkListResponse res;

        this->link_list(request["pattern"].asString(), res);

        response["pattern"] = request["pattern"].asString();
        response["data"] = Json::arrayValue;

        for (auto i : res.list) {

            Json::Value item;
            Json::Value data;

            if (i.has_data) {

                data["offset"] = i.data_item.offset.to_string();
                data["type"] = i.data_item.type.to_string();
                data["semaphore"] = i.data_item.semaphore;
                data["private"] = i.data_item.isPrivate;
                item["data"] = data;

            }

            item["key"] = i.key;
            item["id"] = i.data_item.id;

            response["data"].append(item);

        }

        spdlog::debug("Response {}", response.toStyledString());

    }


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

    struct ConfigResponse {
        std::string profile;
        std::string version;
        std::string shmFile;
        unsigned int size;
        unsigned int defaultCycle; // in us
    };

    struct MemoryAllocResponse {
        unsigned int offset;
        std::string token;
        int semaphore;
    };

    struct MemoryListResponse {
        std::vector<RikerIO::MemoryAreaPtr> data;
    };

    struct MemoryGetResponse {
        RikerIO::MemoryAreaPtr data;
    };

    struct DataAddRequest {
        std::string token;
        RikerIO::Type type;
        RikerIO::MemoryPosition offset;
    };

    struct DataAddResponse {
        RikerIO::Type type;
        RikerIO::MemoryPosition offset;
        int semaphore;
    };

    struct DataRemoveResponse {
        unsigned int count;
    };

    struct DataListResponseItem {
        std::string id;
        RikerIO::MemoryPosition offset;
        const RikerIO::Type type;
        int semaphore;
        bool isPrivate;
    };

    struct DataListResponse {
        std::vector<DataListResponseItem> list;
    };

    struct LinkListItem {
        std::string key;
        bool has_data;
        DataListResponseItem data_item;
    };

    struct LinkListResponse {
        std::vector<LinkListItem> list;
    };

    virtual void get_config(ConfigResponse&) = 0;

    virtual void memory_alloc(int, MemoryAllocResponse&) = 0;
    virtual void memory_dealloc(const std::string& token) = 0;
    virtual void memory_list(MemoryListResponse&) = 0;
    virtual void memory_get(int mId, MemoryGetResponse&) = 0;

    virtual void data_add(
        const std::string& id,
        DataAddRequest&,
        DataAddResponse&) = 0;

    virtual void data_remove(
        const std::string& pattern,
        const std::string& token,
        DataRemoveResponse& res) = 0;

    virtual void data_list(
        const std::string& pattern,
        DataListResponse& response) = 0;

    virtual void link_add(
        const std::string& linkname,
        std::vector<std::string>& dataIds,
        unsigned int&) = 0;

    virtual void link_remove(
        const std::string& linkname,
        std::vector<std::string>& data_ids,
        unsigned int&) = 0;

    virtual void link_list(
        const std::string& pattern,
        LinkListResponse&) = 0;

};

#endif //JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_
