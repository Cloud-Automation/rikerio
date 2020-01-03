#ifndef __RIKERIO_LINK_ADD_REQUEST_H__
#define __RIKERIO_LINK_ADD_REQUEST_H__

#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {

class LinkAdd : public RPCRequest<1> {

  public:
    LinkAdd(const std::string& key, const std::vector<std::string>& list);
    Json::Value create_params() override;

  private:
    const std::string key;
    const std::vector<std::string> list;


};

}
}
}


#endif
