#include "client/request/link-add.h"

RikerIO::Request::v1::LinkAdd::LinkAdd(const std::string& key, const std::vector<std::string>& list) :
    RPCRequest<1>(),
    key(key),
    list(list) {}

Json::Value RikerIO::Request::v1::LinkAdd::create_params() {

    Json::Value result;
    Json::Value data = Json::arrayValue;
    result["key"] = key;

    for (auto l : list) {
        data.append(l);
    }
    result["data"] = data;

    return result;

}
