#include "client/client.h"

std::shared_ptr<RikerIO::AbstractResponse> cmd_link_remove(RikerIO::Client& client,
        const std::string& pattern,
        std::vector<std::string>& list) {

    RikerIO::Request::v1::LinkRemove req(pattern, list);

    auto response = client.link_remove(req);

    printf("%d\n", response->get_counter());

    return std::static_pointer_cast<RikerIO::AbstractResponse>(response);

}
