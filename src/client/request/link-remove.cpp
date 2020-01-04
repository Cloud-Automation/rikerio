#include "client/request/link-remove.h"

RikerIO::Request::v1::LinkRemove::LinkRemove(const std::string& pattern, std::vector<std::string> list) :
    RPCRequest<1>(), pattern(pattern), list(list) {
}

Json::Value RikerIO::Request::v1::LinkRemove::create_params () {

    Json::Value result;

    result["pattern"] = pattern;
    result["list"] = Json::arrayValue;

    for (auto l : list) {
        result["list"].append(l);
    }

    return result;

}