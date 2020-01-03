#include "client/response/link-add.h"

RikerIO::Response::v1::LinkAdd::LinkAdd(Json::Value& result) : RPCResponse(result) {

    if (!ok()) {
        return;
    }

    counter = result["data"]["counter"].asUInt();

}

unsigned int RikerIO::Response::v1::LinkAdd::get_counter() const {
    return counter;
}
