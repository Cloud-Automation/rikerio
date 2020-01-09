#include "client/response/data-add.h"

RikerIO::Response::v1::DataAdd::DataAdd(Json::Value& result) : RikerIO::Response::v1::DataAdd(result, NULL) { }
RikerIO::Response::v1::DataAdd::DataAdd(Json::Value& result, uint8_t* memory_ptr) :
    RPCResponse(result) {

    if (!ok()) {
        return;
    }

    id = result["data"]["id"].asString();
    offset = RikerIO::MemoryPosition(result["data"]["offset"].asString());
    type = RikerIO::Type(result["data"]["type"].asString());

    data_ptr = memory_ptr + offset.get_byte_offset();

}

const std::string& RikerIO::Response::v1::DataAdd::get_id() const {
    return id;
}

RikerIO::MemoryPosition& RikerIO::Response::v1::DataAdd::get_offset() {
    return offset;
}

const RikerIO::Type& RikerIO::Response::v1::DataAdd::get_type() const {
    return type;
}

uint8_t* RikerIO::Response::v1::DataAdd::get_data_ptr() const {
    return data_ptr;
}
