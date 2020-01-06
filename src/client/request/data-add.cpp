#include "client/request/data-add.h"

RikerIO::Request::v1::DataAdd::DataAdd(
    const std::string& id,
    const std::string& token,
    RikerIO::Utils::Datatype type,
    unsigned int size,
    unsigned int offset,
    unsigned int index) :
    RPCRequest<1>(),
    id(id),
    token(token),
    type(type),
    size(size),
    offset(offset),
    index(index) { }

Json::Value RikerIO::Request::v1::DataAdd::create_params() {

    Json::Value result;
    Json::Value data;

    result["id"] = id;
    if (token != "") {
        data["token"] = token;
    }

    data["type"] = Utils::GetStringFromType(type);
    data["size"] = size;
    data["offset"] = offset;
    data["index"] = index;

    result["data"] = data;

    return result;

}
