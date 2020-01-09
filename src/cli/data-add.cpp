#include "client/client.h"
#include <iostream>

std::shared_ptr<RikerIO::RPCResponse> cmd_data_add(
    RikerIO::Client& client,
    const std::string& id,
    const std::string& token,
    RikerIO::Type& type,
    RikerIO::MemoryPosition& offset) {

    RikerIO::Request::v1::DataAdd request(
        id,
        token,
        type,
        offset);

    return client.data_add(request);

}
