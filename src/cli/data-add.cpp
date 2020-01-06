#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>


std::shared_ptr<RikerIO::RPCResponse> cmd_data_add(
    RikerIO::Client& client,
    const std::string& token,
    const std::string& id,
    const std::string& type,
    unsigned int index,
    unsigned int offset) {

    RikerIO::Request::v1::DataAdd request(
        id,
        token,
        RikerIO::Utils::GetTypeFromString(type),
        RikerIO::Utils::GetBitSize(type),
        offset,
        index);

    return client.data_add(request);

}

std::shared_ptr<RikerIO::RPCResponse> cmd_data_add(
    RikerIO::Client& client,
    const std::string& token,
    const std::string& id,
    unsigned int size,
    unsigned int index,
    unsigned int offset) {

    RikerIO::Request::v1::DataAdd request(
        id,
        token,
        RikerIO::Utils::Datatype::UNDEFINED,
        size,
        offset,
        index);

    return client.data_add(request);

}
