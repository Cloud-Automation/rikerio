#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>

void cmd_memory_alloc(RikerIO::Client& client, unsigned int size, bool tokenOnly) {

    RikerIO::AbstractClient::MemoryAllocResponse response;

    client.memory_alloc(size, response);

    if (tokenOnly) {
        fprintf(stdout, "%s\n", response.token.c_str());
    } else {
        fprintf(stdout, "%d;%s\n", response.offset, response.token.c_str());
    }

    exit(EXIT_SUCCESS);

}
