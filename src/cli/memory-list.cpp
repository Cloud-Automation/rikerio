#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>

void cmd_memory_list(RikerIO::Client& client) {

    RikerIO::AbstractClient::MemoryListResponse response;

    client.memory_list(response);

    printf("OFFSET\tSIZE\tSEMAPHORE\n");

    for (auto a : response.list) {
        printf("%d\t%d\t%d\n", a.offset, a.size, a.semaphore);
    }

    exit(EXIT_SUCCESS);

}
