#ifndef __RIKERIO_DATA_REMOVE_RESPONSE_H__
#define __RIKERIO_DATA_REMOVE_RESPONSE_H__

#include "client/response.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class DataRemove : public RikerIO::RPCResponse {
  public:
    DataRemove(Json::Value& result) : RPCResponse(result) {
        if (!ok()) {
            return;
        }

        count = result["data"]["count"].asUInt();
    };

    unsigned int get_count() const {
        return count;
    }

  private:
    unsigned int count = 0;
};

using DataRemovePtr = std::shared_ptr<DataRemove>;
}
}
}

#endif
