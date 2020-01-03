#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>

std::shared_ptr<RikerIO::RPCResponse> cmd_memory_dealloc(RikerIO::Client& client, const std::string& token) {

    RikerIO::Request::v1::MemoryDealloc req(token);
    auto response  = client.memory_dealloc(req);

    return response;

}
