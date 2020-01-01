#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>

void cmd_config_get(RikerIO::Client& rpcClient) {

    RikerIO::AbstractClient::ConfigGetResponse response;

    rpcClient.config_get(response);

    printf("ID\tVERSION\tSHM-File\t\t\tSize (Bytes)\tCYCLE\n");

    printf("%s\t%s\t%s\t%d\t\t%d\n",
           response.profile.c_str(),
           response.version.c_str(),
           response.shmFile.c_str(),
           response.size,
           response.defaultCycle);

    exit(EXIT_SUCCESS);

}
