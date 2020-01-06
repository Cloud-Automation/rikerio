#include "client/response/link-remove.h"

RikerIO::Response::v1::LinkRemove::LinkRemove(Json::Value& result) : RPCResponse(result) {
    if (!ok()) {
        return;
    }

    count = result["data"]["count"].asUInt();
}

unsigned int RikerIO::Response::v1::LinkRemove::get_count () const {
    return count;
}
