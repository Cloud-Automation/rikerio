#ifndef __RIKERIO_LINK_ADD_RESPONSE_H__
#define __RIKERIO_LINK_ADD_RESPONSE_H__

#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class LinkAdd : public RPCResponse {
  public:
    LinkAdd(Json::Value& result) : RPCResponse(result) {

        if (!ok()) {
            return;
        }

        counter = result["data"]["counter"].asUInt();

    }

    unsigned int get_counter() const {
        return counter;
    }
  private:
    unsigned int counter = 0;
};
using LinkAddPtr = std::shared_ptr<LinkAdd>;
}
}
}


#endif
