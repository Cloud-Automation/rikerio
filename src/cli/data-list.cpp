#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>
#include <algorithm>

bool cmd_data_list_comp_offset(
    std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem> a,
    std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem> b) {

    if (a->get_offset() == b->get_offset()) {
        return a->get_index() < b->get_index();
    }

    return a->get_offset() < b->get_offset();

}

bool cmd_data_list_comp_id(
    std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem> a,
    std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem> b) {

    return a->get_id() < b->get_id();

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

    if (extendedList) {
        printf("OFFSET\tSIZE\tSEMAPHORE\tFLAGS\tTYPE\t\tID\n");

        for (auto a : items) {

            std::string flags = "";
            flags += a->is_private() ? "P" : "-";

            printf("%d.%d\t%d\t%d\t\t%s\t%-10s\t%s\n",
                   a->get_offset(),
                   a->get_index(),
                   a->get_size(),
                   a->get_semaphore(),
                   flags.c_str(),
                   RikerIO::Utils::GetStringFromType(a->get_datatype()).c_str(),
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
