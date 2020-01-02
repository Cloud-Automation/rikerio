#ifndef __RIKERIO_LINK_ADD_REQUEST_H__
#define __RIKERIO_LINK_ADD_REQUEST_H__

#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {

class LinkAdd : public RPCRequest<1> {

  public:
    LinkAdd(const std::string& key, const std::vector<std::string>& list) :
        RPCRequest<1>(),
        key(key),
        list(list) {}

    Json::Value create_params() override {

        Json::Value result;
        Json::Value data = Json::arrayValue;
        result["key"] = key;
        result["data"] = data;

        for (auto l : list) {
            data.append(l);
        }

        return result;

    }

  private:
    const std::string key;
    const std::vector<std::string> list;


};

}
}
}


#endif
