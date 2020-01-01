
#include "client/client.h"

using namespace RikerIO;

Client::Client(jsonrpc::IClientConnector &conn) :
    jsonrpc::Client(conn, jsonrpc::JSONRPC_CLIENT_V2) { }

void Client::config_get(AbstractClient::ConfigGetResponse& response) {

    Json::Value p;
    Json::Value result = CallMethod("v1/configGet", p);
    if (result.isObject())

        if (result["code"].asInt() == 0) {

            response.profile = result["data"]["id"].asString();
            response.version = result["data"]["version"].asString();

            response.shmFile = result["data"]["shm"].asString();
            response.size = result["data"]["size"].asUInt();
            response.defaultCycle = result["data"]["count"].asUInt();

        } else {

            throw ClientError(result["code"].asInt(), result["message"].asString());

        }

    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());


}

void Client::memory_alloc(unsigned int size, MemoryAllocResponse& response) {

    Json::Value p;
    p["size"] = size;
    Json::Value result = CallMethod("v1/memoryAlloc", p);
    if (result.isObject())

        if (result["code"].asInt() == 0) {

            response.token = result["data"]["token"].asString();
            response.offset = result["data"]["offset"].asUInt();

        } else {

            throw ClientError(result["code"].asInt(), result["message"].asString());

        }

    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}


void Client::memory_dealloc(const std::string& token) {
    Json::Value p;
    p["token"] = token;
    Json::Value result = this->CallMethod("v1/memoryDealloc", p);
    if (result.isObject())

        if (result["code"].asInt() == 0) {

            return;

        } else {

            throw ClientError(result["code"].asInt(), result["message"].asString());

        }

    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}


void Client::memory_list(AbstractClient::MemoryListResponse& res) {
    Json::Value p;
    p = Json::nullValue;
    Json::Value result = this->CallMethod("v1/memoryList", p);
    if (result.isObject())

        if (result["code"].asInt() == 0) {

            for (auto a : result["data"]) {
                MemoryListItem item;
                item.offset = a["offset"].asUInt();
                item.size = a["size"].asUInt();
                item.semaphore = a["semaphore"].asInt();
                res.list.push_back(item);
            }

        } else {

            throw ClientError(result["code"].asInt(), result["message"].asString());

        } else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}


void Client::data_create(const std::string& token, const std::string& id, DataCreateRequest request) {
    Json::Value p;

    p["data"] = Json::objectValue;
    p["id"] = id;
    if (token != "") {
        p["token"] = token;
    }

    if (request.type != "") {
        p["data"]["type"] = request.type;
    }

    if (request.size != 0) {
        p["data"]["size"] = request.size;
    }

    //p["data"]["index"] = request.index;
    p["data"]["offset"] = request.offset;


    Json::Value result = this->CallMethod("v1/dataCreate", p);

    if (result.isObject())

        if (result["code"].asInt() == 0) {

            return;

        } else {

            throw ClientError(result["code"].asInt(), result["message"].asString());

        }

    else
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
}

void Client::data_list(const std::string& pattern, DataListResponse& res) {
    Json::Value p;
    p["pattern"] = pattern;
    Json::Value result = this->CallMethod("v1/dataList", p);
    if (result.isObject())

        if (result["code"].asInt() == 0) {

            for (auto a : result["data"]) {

                DataListItem item;

                item.id = a["id"].asString();
                item.datatype = a["type"].asString();
                item.offset = a["offset"].asUInt();
                item.index = a["index"].asUInt();
                item.size = a["size"].asUInt();
                item.semaphore = a["semaphore"].asInt();
                item.isPrivate = a["private"].asBool();
                res.list.push_back(item);

            }

        } else {

            throw ClientError(result["code"].asInt(), result["message"].asString());

        } else {
        throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
    }
}

void Client::data_remove(const std::string& token, const std::string& pattern, unsigned int& count) {

    Json::Value p;
    if (token.length() != 0) {
        p["token"] = token;
    }
    p["pattern"] = pattern;
    Json::Value result = this->CallMethod("v1/dataRemove", p);
    if (result.isObject()) {

        if (result["code"].asInt() == 0 && result["data"]) {

            count = result["data"]["count"].asUInt();

        } else {

            throw ClientError(
                result["code"].asInt(),
                result["message"].asString());
        }

    } else {

        throw jsonrpc::JsonRpcException(
            jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
            result.toStyledString());

    }

}




void Client::link_add(const std::string& key, std::vector<std::string>& list, unsigned int& counter) {

    Json::Value p;
    p["key"] = key;
    p["data"] = Json::arrayValue;
    for (auto l : list) {
        p["data"].append(l);
    }
    Json::Value result = this->CallMethod("v1/linkAdd", p);

    if (result.isObject()) {

        if (result["code"].asInt() == 0 && result["data"]) {

            counter = result["data"]["counter"].asUInt();

        } else {

            throw ClientError(
                result["code"].asInt(),
                result["message"].asString());
        }

    } else {

        throw jsonrpc::JsonRpcException(
            jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
            result.toStyledString());

    }

}

void Client::link_list(const std::string& pattern, LinkListResponse& response) {
    Json::Value p;
    p["pattern"] = pattern;
    Json::Value result = this->CallMethod("v1/linkList", p);
    if (result.isObject()) {

        if (result["code"].asInt() == 0 && result["data"]) {

            for (auto a : result["data"]) {

                LinkListItem linkItem;

                linkItem.key = a["key"].asString();
                linkItem.id = a["id"].asString();

                if (!a["data"]) {
                    linkItem.data = nullptr;
                    response.list.push_back(linkItem);
                    continue;
                }

                std::shared_ptr<DataListItem> item = std::make_shared<DataListItem>();

                item->id = linkItem.id;
                item->datatype = a["data"]["type"].asString();
                item->offset = a["data"]["offset"].asUInt();
                item->index = a["data"]["index"].asUInt();
                item->size = a["data"]["size"].asUInt();
                item->semaphore = a["data"]["semaphore"].asInt();
                item->isPrivate = a["data"]["private"].asBool();
                linkItem.data = item;

                response.list.push_back(linkItem);
            }

        } else {

            throw ClientError(
                result["code"].asInt(),
                result["message"].asString());
        }

    } else {

        throw jsonrpc::JsonRpcException(
            jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
            result.toStyledString());

    }

}


#if 0

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

#endif
