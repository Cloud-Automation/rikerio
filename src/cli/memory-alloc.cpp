#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>

std::shared_ptr<RikerIO::RPCResponse> cmd_memory_alloc(
    RikerIO::Client& client,
    unsigned int size,
    bool tokenOnly) {


    RikerIO::Request::v1::MemoryAlloc request(size);
    auto response = client.memory_alloc(request);

    if (response->ok()) {
        if (tokenOnly) {
            fprintf(stdout, "%s\n", response->get_token().c_str());
        } else {
            fprintf(stdout, "%d;%s\n", response->get_offset(), response->get_token().c_str());
        }

    }

    return response;

}
