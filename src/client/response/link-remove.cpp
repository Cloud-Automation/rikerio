#include "client/response/link-remove.h"

RikerIO::Response::v1::LinkRemove::LinkRemove(Json::Value& result) : RPCResponse(result) {
    if (!ok()) {
        return;
    }

    counter = result["data"]["count"].asUInt();
}

unsigned int RikerIO::Response::v1::LinkRemove::get_counter () const {
    return counter;
}
