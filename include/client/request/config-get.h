#ifndef __RIKERIO_CONFIG_GET_REQUEST_H__
#define __RIKERIO_CONFIG_GET_REQUEST_H__

#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {
class ConfigGet : public RikerIO::RPCRequest<1> {
};
}
}
}

#endif
