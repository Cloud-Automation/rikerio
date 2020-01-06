#ifndef __RIKERIO_LINK_REMOVE_RESPONSE_H__
#define __RIKERIO_LINK_REMOVE_RESPONSE_H__

#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {

class LinkRemove : public RPCResponse {

  public:
    LinkRemove(Json::Value& result);
    unsigned int get_count () const;

  private:
    unsigned int count = 0;

};
using LinkRemovePtr = std::shared_ptr<LinkRemove>;
}
}
}

#endif
