#include "client/response/data-remove.h"

RikerIO::Response::v1::DataRemove::DataRemove(Json::Value& result) : RPCResponse(result) {

    if (!ok()) {
        return;
    }

    count = result["data"]["count"].asUInt();

}

unsigned int RikerIO::Response::v1::DataRemove::get_count() const {
    return count;
}
