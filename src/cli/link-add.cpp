#include "client/client.h"
#include <iostream>

std::shared_ptr<RikerIO::RPCResponse> cmd_link_add(RikerIO::Client& client,
        const std::string& key,
        std::vector<std::string>& list) {


    RikerIO::Request::v1::LinkAdd req(key, list);

    auto response = client.link_add(req);

    std::cout << response->get_count() << std::endl;

    return response;

}
