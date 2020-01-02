#include "client/client.h"

std::shared_ptr<RikerIO::AbstractResponse> cmd_link_add(RikerIO::Client& client,
        const std::string& key,
        std::vector<std::string>& list) {


    RikerIO::Request::v1::LinkAdd req(key, list);

    auto response = client.link_add(req);

    printf("%d\n", response->get_counter());

    return std::static_pointer_cast<RikerIO::AbstractResponse>(response);

}
