#ifndef __RIKERIO_DATA_LIST_REQUEST_H__
#define __RIKERIO_DATA_LIST_REQUEST_H__

#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {

class DataList : public RPCRequest<1> {

  public:

    DataList(const std::string& pattern);
    Json::Value create_params() override;

  private:

    const std::string pattern;

};

}
}
}


#endif
