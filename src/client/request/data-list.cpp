#include "client/request/data-list.h"

RikerIO::Request::v1::DataList::DataList(const std::string& pattern) :
    RPCRequest<1>(), pattern(pattern) {}

Json::Value RikerIO::Request::v1::DataList::create_params() {
    Json::Value result;
    result["pattern"] = pattern;
    return result;
}
