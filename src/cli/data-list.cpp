#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>
#include <algorithm>

bool cmd_data_list_comp_offset(
    std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem> a,
    std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem> b) {

    return a < b;

}

bool cmd_data_list_comp_id(
    std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem> a,
    std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem> b) {

    return a->get_id() < b->get_id();

}

std::string ws_exp(const std::string& str, int maxLen)  {

    std::string tmp = "";

    for (int i = 0; i < (maxLen - (int) str.length()); i +=1 ) {
        tmp += " ";
    }

    tmp = str + tmp;

    return tmp;

}

std::shared_ptr<RikerIO::RPCResponse> cmd_data_list(
    RikerIO::Client& client,
    const std::string& pattern,
    bool extendedList,
    const std::string sortBy,
    bool descending) {


    RikerIO::Request::v1::DataList req(pattern);

    auto response = client.data_list(req);
    auto items = response->get_items();

    if (sortBy == "offset") {
        sort(items.begin(), items.end(), cmd_data_list_comp_offset);
    } else if (sortBy == "id") {
        sort(items.begin(), items.end(), cmd_data_list_comp_id);
    }

    if (descending) {
        std::reverse(items.begin(), items.end());
    }

    unsigned int maxOffsetLen = 7;
    unsigned int maxTypeLen = 4;

    for (auto a : items) {
        maxOffsetLen = std::max((unsigned int)a->get_offset().to_string().length(), maxOffsetLen);
        maxTypeLen = std::max((unsigned int)a->get_type().to_string().length(), maxTypeLen);
    }

    if (extendedList) {
        printf("%s %s SEMAPHORE FLAGS ID\n",
               ws_exp("OFFSET", maxOffsetLen).c_str(),
               ws_exp("TYPE", maxTypeLen).c_str());

        for (auto a : items) {

            std::string flags = "";
            flags += a->is_private() ? "P" : "-";

            printf("%s %s %s %-5s %s\n",
                   ws_exp(a->get_offset().to_string(), maxOffsetLen).c_str(),
                   ws_exp(a->get_type().to_string(), maxTypeLen).c_str(),
                   ws_exp(std::to_string(a->get_semaphore()->get_id()), 9).c_str(),
                   flags.c_str(),
                   a->get_id().c_str());
        }

    } else {
        for (auto a : items) {
            printf("%s\n",
                   a->get_id().c_str());
        }


    }

    return response;

}
