#ifndef __RIKERIO_DATA_ADD_RESPONSE_H__
#define __RIKERIO_DATA_ADD_RESPONSE_H__

#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class DataAdd : public RikerIO::RPCResponse {
  public:
    DataAdd(Json::Value& result) : RPCResponse(result) { }
};

using DataAddPtr = std::shared_ptr<DataAdd>;

}
}
}


#endif
