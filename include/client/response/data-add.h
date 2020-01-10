#ifndef __RIKERIO_DATA_ADD_RESPONSE_H__
#define __RIKERIO_DATA_ADD_RESPONSE_H__

#include "client/response.h"
#include "common/type.h"
#include "common/mem-position.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class DataAdd : public RikerIO::RPCResponse {
  public:
    DataAdd(Json::Value& result);
    DataAdd(Json::Value& result, uint8_t* memory_ptr);

    const std::string& get_id() const;
    MemoryPosition& get_offset();
    const Type& get_type() const;

    uint8_t* get_data_ptr() const;

  private:

    std::string id;
    MemoryPosition offset;
    Type type;

    uint8_t* data_ptr = NULL;
};

using DataAddPtr = std::shared_ptr<DataAdd>;

}
}
}


#endif
