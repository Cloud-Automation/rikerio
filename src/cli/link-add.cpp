#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>

void cmd_link_add(RikerIO::Client& client,
                  const std::string& key,
                  std::vector<std::string>& list) {


    unsigned int counter = 0;

    client.link_add(key, list, counter);

    printf("%d\n", counter);

    exit(EXIT_SUCCESS);

}
