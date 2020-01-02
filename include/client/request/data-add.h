#ifndef __RIKERIO_DATA_ADD_REQUEST_H__
#define __RIKERIO_DATA_ADD_REQUEST_H__

#include "common/utils.h"
#include "client/request.h"

namespace RikerIO {
namespace Request {
namespace v1 {
class DataAdd : public RikerIO::RPCRequest<1> {
  public:
    DataAdd(
        const std::string& id,
        const std::string& token,
        RikerIO::Utils::Datatype type,
        unsigned int size,
        unsigned int offset,
        unsigned int index) :
        RPCRequest<1>(),
        id(id),
        token(token),
        type(type),
        size(size),
        offset(offset),
        index(index) { };

    Json::Value create_params() {

        Json::Value result;
        Json::Value data;

        result["id"] = id;
        result["data"] = data;
        if (token != "") {
            data["token"] = token;
        }

        data["type"] = Utils::GetStringFromType(type);
        data["size"] = size;
        data["offset"] = offset;
        data["index"] = index;

        return result;

    };

  private:
    const std::string& id;
    const std::string& token;
    RikerIO::Utils::Datatype type;
    unsigned int size;
    unsigned int offset;
    unsigned int index;
};
}
}
}


#endif
