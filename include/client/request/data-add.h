#ifndef __RIKERIO_DATA_ADD_REQUEST_H__
#define __RIKERIO_DATA_ADD_REQUEST_H__

#include "common/type.h"
#include "common/mem-position.h"
#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {
class DataAdd : public RikerIO::RPCRequest<1> {
  public:

    DataAdd(
        const std::string& id,
        const std::string& token,
        RikerIO::Type type,
        RikerIO::MemoryPosition offset);

    Json::Value create_params();

  private:
    const std::string& id;
    const std::string& token;
    RikerIO::Type type;
    RikerIO::MemoryPosition offset;
};
}
}
}


#endif
