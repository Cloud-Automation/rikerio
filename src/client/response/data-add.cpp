#include "client/response/data-add.h"

RikerIO::Response::v1::DataAdd::DataAdd(Json::Value& result) : RikerIO::Response::v1::DataAdd(result, NULL) { }
RikerIO::Response::v1::DataAdd::DataAdd(Json::Value& result, uint8_t* memory_ptr) :
    RPCResponse(result) {

    if (!ok()) {
        return;
    }

    id = result["data"]["id"].asString();
    offset = result["data"]["offset"].asUInt();
    index = result["data"]["index"].asUInt();
    size = result["data"]["size"].asUInt();
    type = RikerIO::Utils::GetTypeFromString(result["data"]["type"].asString());

    data_ptr = memory_ptr + offset;

}

const std::string& RikerIO::Response::v1::DataAdd::get_id() const {
    return id;
}

unsigned int RikerIO::Response::v1::DataAdd::get_offset() const {
    return offset;
}

unsigned int RikerIO::Response::v1::DataAdd::get_index() const {
    return index;
}

unsigned int RikerIO::Response::v1::DataAdd::get_size() const {
    return size;
}

RikerIO::Utils::Datatype RikerIO::Response::v1::DataAdd::get_type() const {
    return type;
}

uint8_t* RikerIO::Response::v1::DataAdd::get_data_ptr() const {
    return data_ptr;
}
