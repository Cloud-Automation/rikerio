#ifndef __RIKERIO_LINK_ADD_RESPONSE_H__
#define __RIKERIO_LINK_ADD_RESPONSE_H__

#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class LinkAdd : public RPCResponse {
  public:

    LinkAdd(Json::Value& result);
    unsigned int get_counter() const;

  private:
    unsigned int counter = 0;
};
using LinkAddPtr = std::shared_ptr<LinkAdd>;
}
}
}


#endif
