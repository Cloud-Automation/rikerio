#ifndef __RIKERIO_DATA_REMOVE_REQUEST_H__
#define __RIKERIO_DATA_REMOVE_REQUEST_H__

#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {
class DataRemove : public RikerIO::RPCRequest<1> {
  public:

    DataRemove(const std::string& pattern, const std::string& token);
    Json::Value create_params() override;

  private:
    const std::string pattern;
    const std::string token;

};
}
}
}

#endif
