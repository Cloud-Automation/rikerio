#include "client/client.h"
#include <iostream>

std::shared_ptr<RikerIO::RPCResponse> cmd_link_remove(RikerIO::Client& client,
        const std::string& pattern,
        std::vector<std::string>& list) {

    RikerIO::Request::v1::LinkRemove req(pattern, list);

    auto response = client.link_remove(req);

    std::cout << response->get_count() << std::endl;

    return response;

}
