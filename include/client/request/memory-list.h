#ifndef __RIKERIO_MEMORY_LIST_REQUEST_H__
#define __RIKERIO_MEMORY_LIST_REQUEST_H__

#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {
class MemoryList : public RikerIO::RPCRequest<1> { };
}
}
}

#endif
