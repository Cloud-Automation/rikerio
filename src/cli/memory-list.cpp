#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>

std::shared_ptr<RikerIO::RPCResponse> cmd_memory_list(RikerIO::Client& client) {

    RikerIO::Request::v1::MemoryList request;

    auto response = client.memory_list(request);

    if (response->ok()) {

        printf("OFFSET\tSIZE\tSEMAPHORE\n");

        for (auto a : response->get_items()) {
            printf("%d\t%d\t%d\n", a->get_offset(), a->get_size(), a->get_semaphore()->get_id());
        }

    }

    return response;

}
