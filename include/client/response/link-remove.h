#ifndef __RIKERIO_LINK_REMOVE_RESPONSE_H__
#define __RIKERIO_LINK_REMOVE_RESPONSE_H__

#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {

class LinkRemove : public RPCResponse {

  public:
    LinkRemove(Json::Value& result) : RPCResponse(result) {
        if (!ok()) {
            return;
        }

        counter = result["data"]["count"].asUInt();
    }

    unsigned int get_counter () const {
        return counter;
    }

  private:
    unsigned int counter = 0;

};
using LinkRemovePtr = std::shared_ptr<LinkRemove>;
}
}
}

#endif
