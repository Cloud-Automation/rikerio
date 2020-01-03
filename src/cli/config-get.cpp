#include "client/client.h"
#include "client/response.h"
#include "client/response/config-get.h"
#include <iostream>

std::shared_ptr<RikerIO::RPCResponse> cmd_config_get(RikerIO::Client& client) {

    RikerIO::Request::v1::ConfigGet request;
    auto response = client.config_get(request);

    if (response->ok()) {

        printf("ID\tVERSION\tSHM-File\t\t\tSize (Bytes)\tCYCLE\n");

        printf("%s\t%s\t%s\t%d\t\t%d\n",
               response->get_profile().c_str(),
               response->get_version().c_str(),
               response->get_shm_file().c_str(),
               response->get_size(),
               response->get_cycle());

    }

    return response;

}
