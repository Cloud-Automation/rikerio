#ifndef __RIKERIO_LINK_REMOVE_REQUEST_H__
#define __RIKERIO_LINK_REMOVE_REQUEST_H__

#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {
class LinkRemove : public RPCRequest<1> {

  public:
    LinkRemove(const std::string& pattern, std::vector<std::string> list) :
        RPCRequest<1>(), pattern(pattern), list(list) {
    }

    Json::Value create_params () override {

        Json::Value result;

        result["pattern"] = pattern;
        result["list"] = Json::arrayValue;

        for (auto l : list) {
            result["list"].append(l);
        }

        return result;

    }

  private:

    const std::string pattern;
    const std::vector<std::string> list;

};
}
}
}


#endif
