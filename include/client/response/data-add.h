#ifndef __RIKERIO_DATA_ADD_RESPONSE_H__
#define __RIKERIO_DATA_ADD_RESPONSE_H__

#include "client/response.h"
#include "common/utils.h"

namespace RikerIO {
namespace Response {
namespace v1 {
class DataAdd : public RikerIO::RPCResponse {
  public:
    DataAdd(Json::Value& result);
    DataAdd(Json::Value& result, uint8_t* memory_ptr);

    const std::string& get_id() const;
    unsigned int get_offset() const;
    unsigned int get_index() const;
    unsigned int get_size() const;
    RikerIO::Utils::Datatype get_type() const;

    uint8_t* get_data_ptr() const;

  private:

    std::string id;
    unsigned int offset;
    unsigned int index;
    unsigned int size;
    RikerIO::Utils::Datatype type;

    uint8_t* data_ptr = NULL;
};

using DataAddPtr = std::shared_ptr<DataAdd>;

}
}
}


#endif
