#include "client/response/link-add.h"

RikerIO::Response::v1::LinkAdd::LinkAdd(Json::Value& result) : RPCResponse(result) {

    if (!ok()) {
        return;
    }

    count = result["data"]["count"].asUInt();

}

unsigned int RikerIO::Response::v1::LinkAdd::get_count() const {
    return count;
}
