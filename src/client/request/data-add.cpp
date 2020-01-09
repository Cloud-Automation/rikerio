#include "client/request/data-add.h"


RikerIO::Request::v1::DataAdd::DataAdd(
    const std::string& id,
    const std::string& token,
    RikerIO::Type type,
    RikerIO::MemoryPosition offset) :
    RPCRequest<1>(),
    id(id),
    token(token),
    type(type),
    offset(offset) { }

Json::Value RikerIO::Request::v1::DataAdd::create_params() {

    Json::Value result;
    Json::Value data;

    if (token != "") {
        data["token"] = token;
    }

    data["id"] = id;
    data["type"] = type.to_string();
    data["offset"] = offset.to_string();

    result["data"] = data;

    return result;

}
