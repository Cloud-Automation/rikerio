#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>

void cmd_memory_dealloc(RikerIO::Client& client, const std::string& token) {

    client.memory_dealloc(token);

    exit(EXIT_SUCCESS);

}
