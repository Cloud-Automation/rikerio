#ifndef __RIKERIO_LINK_LIST_REQUEST_H__
#define __RIKERIO_LINK_LIST_REQUEST_H__

#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {

class LinkList : public RPCRequest<1> {
  public:
    LinkList(const std::string& pattern) : RPCRequest<1>(), pattern(pattern) { }
    Json::Value create_params () override {
        Json::Value result;
        result["pattern"] = pattern;
        return result;
    }
  private:
    const std::string pattern;
};

}
}
}

#endif
