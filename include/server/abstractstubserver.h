
#ifndef JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_
#define JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_

#include <jsonrpccpp/server.h>
#include <algorithm>

#include "common/error.h"
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
                               "data", jsonrpc::JSON_OBJECT,
                               "id", jsonrpc::JSON_STRING, NULL),
            &AbstractStubServer::data_createI);

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
            jsonrpc::Procedure("v1/dataGet", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_STRING, "dId",
                               jsonrpc::JSON_OBJECT, NULL),
            &AbstractStubServer::data_getI);

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/dataRemove", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_STRING, "pattern",
                               jsonrpc::JSON_OBJECT, NULL),
            &AbstractStubServer::data_getI);

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

        this->bindAndAddMethod(
            jsonrpc::Procedure("v1/linkGet", jsonrpc::PARAMS_BY_NAME,
                               jsonrpc::JSON_STRING, "lId",
                               jsonrpc::JSON_OBJECT, NULL),
            &AbstractStubServer::link_getI);

    }


    inline virtual void configI(const Json::Value& /*request */, Json::Value& response) {

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

        }  catch (ServerError e) {

            response["code"] = e.getCode();
            response["message"] = e.getMsg();

        }


    }

    inline virtual void memory_allocI(const Json::Value &request, Json::Value &response) {

        try {

            MemoryAllocResponse res;

            this->memory_alloc(request["size"].asInt(), res);

            response["code"] = RikerIO::Error::NO_ERROR;
            response["data"] = Json::objectValue;
            response["data"]["offset"] = res.offset;
            response["data"]["token"] = res.token;

        } catch (ServerError e) {

            response["code"] = e.getCode();
            response["message"] = e.getMsg();

        }

    }

    inline virtual void memory_deallocI(const Json::Value &request, Json::Value &response) {

        try {

            this->memory_dealloc(request["token"].asString());

            response["code"] = RikerIO::Error::NO_ERROR;

        } catch (ServerError e) {

            response["code"] = e.getCode();
            response["message"] = e.getMsg();

        }

    }

    inline virtual void memory_listI(const Json::Value &/*request*/, Json::Value &response) {

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

    }

    inline virtual void memory_getI(const Json::Value& request, Json::Value &response) {

        try {

            MemoryGetResponse res;

            memory_get(request["offset"].asInt(), res);

            Json::Value item;
            item["offset"] = res.data->getOffset();
            item["size"] = res.data->getSize();
            item["semaphore"] = res.data->getSemaphore();

            response["code"] = RikerIO::NO_ERROR;
            response["data"] = item;

        } catch (ServerError e) {
            response["code"] = e.getCode();
            response["message"] = e.getMsg();
        }

    }

    inline virtual void data_createI(const Json::Value &request, Json::Value &response) {

        std::string token = (request["token"] && request["token"].isString()) ?
                            request["token"].asString() : "";

        std::string dId = request["id"].asString();
        //std::transform(dId.begin(), dId.end(), dId.begin(), ::tolower);

        Json::Value data = request["data"];

        bool hasType = data["type"] && data["type"].isString();
        bool hasOffset = data["offset"] && data["offset"].isUInt();
        bool hasIndex = data["index"] && data["index"].isUInt();
        bool hasSize = data["size"] && data["size"].isUInt();

        DataCreateRequest req = { RikerIO::Utils::UNDEFINED, 0, 0, 0 };

        if (hasType && !hasSize && hasOffset) {
            req.type = RikerIO::Utils::GetTypeFromString(data["type"].asString());
            req.size = RikerIO::Utils::DatatypeSize[req.type];
            req.offset = data["offset"].asUInt();
        } else if (!hasType && hasSize && hasOffset) {
            req.type = RikerIO::Utils::UNDEFINED;
            req.size = data["size"].asUInt();
            req.offset = data["offset"].asUInt();
        }

        if (hasIndex) {
            req.index = data["index"].asUInt();

            if (req.size > (7 - req.index) || req.index > 7) {
                response["code"] = RikerIO::BAD_REQUEST;
                response["message"] = "Index violates byte constraints.";
                return;
            }

        }

        try {

            data_create(token, dId, req);
            response["code"] = RikerIO::NO_ERROR;

        } catch (ServerError e) {
            response["code"] = e.getCode();
            response["message"] = e.getMsg();
        }


    }


    inline virtual void data_removeI(const Json::Value &request, Json::Value &response) {

        try {

            std::string token = (request["token"] && request["token"].isString()) ?
                                request["token"].asString() : "";

            DataRemoveResponse res;

            data_remove(request["pattern"].asString(), token, res);

            response["code"] = RikerIO::NO_ERROR;
            response["data"] = Json::objectValue;
            response["data"]["count"] = res.count;

        } catch (ServerError e) {
            response["code"] = e.getCode();
            response["message"] = e.getMsg();
        }

    }


    inline virtual void data_listI(const Json::Value &request, Json::Value &response) {

        try {

            Json::Value resData = Json::arrayValue;

            DataListResponse res;
            data_list(request["pattern"].asString(), res);

            Json::Value item;

            for (auto d : res.list) {

                item["id"] = d.dId;
                item["offset"] = d.offset;
                item["index"] = d.index;
                item["size"] = d.size;
                item["type"] = d.type;
                item["semaphore"] = d.semaphore;
                item["private"] = d.isPrivate;

                resData.append(item);
            }

            response["code"] = 0;
            response["data"] = resData;

        } catch (ServerError e) {

            response["code"] = e.getCode();
            response["message"] = e.getMsg();

        }


    }


    inline virtual void data_getI(const Json::Value &request, Json::Value &response) {
        response = this->data_get(request["id"].asString());
    }
    inline virtual void link_addI(const Json::Value &request, Json::Value &response) {

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

    }

    inline virtual void link_removeI(const Json::Value &request, Json::Value &response) {
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

    }

    inline virtual void link_listI(const Json::Value &request, Json::Value &response) {

        LinkListResponse res;

        this->link_list(request["pattern"].asString(), res);

        response["pattern"] = request["pattern"].asString();
        response["data"] = Json::arrayValue;

        for (auto i : res.list) {

            Json::Value item;
            Json::Value data;

            if (i.has_data) {

                data["offset"] = i.data_item.offset;
                data["index"] = i.data_item.index;
                data["size"] = i.data_item.size;
                data["type"] = i.data_item.type;
                data["semaphore"] = i.data_item.semaphore;
                data["private"] = i.data_item.isPrivate;
                item["data"] = data;

            }

            item["key"] = i.key;
            item["id"] = i.data_item.dId;

            response["data"].append(item);

        }

    }


    inline virtual void link_getI(const Json::Value &request, Json::Value &response) {
        response = this->link_get(request["id"].asString());
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
    };

    struct MemoryListResponse {
        std::vector<RikerIO::MemoryAreaPtr> data;
    };

    struct MemoryGetResponse {
        RikerIO::MemoryAreaPtr data;
    };

    struct DataCreateRequest {
        RikerIO::Utils::Datatype type;
        unsigned int size;
        unsigned int index;
        unsigned int offset;
    };

    struct DataRemoveResponse {
        unsigned int count;
    };

    struct DataListResponseItem {
        std::string dId;
        unsigned int offset;
        unsigned int index;
        unsigned int size;
        std::string type;
        unsigned int semaphore;
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

    virtual void data_create(
        const std::string& token,
        const std::string& dId,
        const DataCreateRequest&) = 0;

    virtual void data_remove(
        const std::string& pattern,
        const std::string& token,
        DataRemoveResponse& res) = 0;

    virtual void data_list(
        const std::string& pattern,
        DataListResponse& response) = 0;

    virtual Json::Value data_get(const std::string& dId) = 0;

    virtual void link_add(
        const std::string& linkname,
        std::vector<std::string>& dataIds,
        unsigned int&) = 0;

    virtual void link_remove(
        const std::string& linkname,
        std::vector<std::string>& data_ids,
        unsigned int&) = 0;

    virtual void link_list(const std::string& pattern, LinkListResponse&) = 0;

    virtual Json::Value link_get(const std::string& lId) = 0;

};

#endif //JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_
