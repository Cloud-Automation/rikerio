#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>


void cmd_data_create(
    RikerIO::Client& client,
    const std::string& token,
    const std::string& id,
    const std::string& type,
    unsigned int index,
    unsigned int offset) {


    RikerIO::AbstractClient::DataCreateRequest request;

    request.type = type;
    request.size = 0;
    request.index = index;
    request.offset = offset;

    client.data_create(token, id, request);

    exit(EXIT_SUCCESS);

}

void cmd_data_create(
    RikerIO::Client& client,
    const std::string& token,
    const std::string& id,
    unsigned int size,
    unsigned int index,
    unsigned int offset) {


    RikerIO::AbstractClient::DataCreateRequest request;

    request.type = "";
    request.size = size;
    request.index = index;
    request.offset = offset;

    client.data_create(token, id, request);

    exit(EXIT_SUCCESS);

}
