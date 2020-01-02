#ifndef __RIKERIO_MEMORY_ALLOC_REQUEST_H__
#define __RIKERIO_MEMORY_ALLOC_REQUEST_H__

#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {
class MemoryAlloc : public RikerIO::RPCRequest<1> {
  public:
    MemoryAlloc(unsigned int size)  : size(size) { }

    Json::Value create_params() override {
        Json::Value params;
        params["size"] = size;
        return params;
    }

  private:
    const unsigned int size;
};
}
}
}


#endif
