#include "client/request/data-remove.h"

RikerIO::Request::v1::DataRemove::DataRemove(const std::string& pattern, const std::string& token = "") :
    RPCRequest<1>(), pattern(pattern), token(token) { }

Json::Value RikerIO::Request::v1::DataRemove::create_params() {
    Json::Value result;

    if (token != "") {
        result["token"] = token;
    }
    result["data"] = Json::objectValue;
    result["data"]["pattern"] = pattern;

    return result;

}
