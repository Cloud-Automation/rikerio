#include "client/request/link-list.h"

RikerIO::Request::v1::LinkList::LinkList(const std::string& pattern) : RPCRequest<1>(), pattern(pattern) { }

Json::Value RikerIO::Request::v1::LinkList::create_params () {
    Json::Value result;
    result["pattern"] = pattern;
    return result;
}
