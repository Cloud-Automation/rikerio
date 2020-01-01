#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>

void cmd_data_remove(
    RikerIO::Client& client,
    const std::string& token,
    const std::string& pattern) {

    unsigned int removeCount = 0;

    client.data_remove(token, pattern, removeCount);

    printf("%d\n", removeCount);

    exit(EXIT_SUCCESS);

}
