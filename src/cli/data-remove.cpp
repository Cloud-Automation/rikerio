#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>

std::shared_ptr<RikerIO::AbstractResponse> cmd_data_remove(
    RikerIO::Client& client,
    const std::string& token,
    const std::string& pattern) {

    RikerIO::Request::v1::DataRemove req(pattern, token);

    auto response = client.data_remove(req);

    printf("%d\n", response->get_count());

    return std::static_pointer_cast<RikerIO::AbstractResponse>(response);

}
