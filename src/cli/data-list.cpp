#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>
#include <algorithm>

bool cmd_data_list_comp_offset(RikerIO::AbstractClient::DataListItem& a, RikerIO::AbstractClient::DataListItem& b) {

    if (a.offset == b.offset) {
        return a.index < b.index;
    }

    return a.offset < b.offset;

}

bool cmd_data_list_comp_id(RikerIO::AbstractClient::DataListItem& a, RikerIO::AbstractClient::DataListItem& b) {

    return a.id < b.id;

}

void cmd_data_list(
    RikerIO::Client& client,
    const std::string& pattern,
    bool extendedList,
    const std::string sortBy,
    bool descending) {


    RikerIO::AbstractClient::DataListResponse response;

    client.data_list(pattern, response);

    if (sortBy == "offset") {
        sort(response.list.begin(), response.list.end(), cmd_data_list_comp_offset);
    } else if (sortBy == "id") {
        sort(response.list.begin(), response.list.end(), cmd_data_list_comp_id);
    }

    if (descending) {
        std::reverse(response.list.begin(), response.list.end());
    }

    if (extendedList) {
        printf("OFFSET\tSIZE\tSEMAPHORE\tFLAGS\tTYPE\t\tID\n");

        for (auto a : response.list) {

            std::string flags = "";
            flags += a.isPrivate ? "P" : "-";

            printf("%d.%d\t%d\t%d\t\t%s\t%-10s\t%s\n",
                   a.offset,
                   a.index,
                   a.size,
                   a.semaphore,
                   flags.c_str(),
                   a.datatype.c_str(),
                   a.id.c_str());
        }

    } else {
        for (auto a : response.list) {
            printf("%s\n",
                   a.id.c_str());
        }


    }

    exit(EXIT_SUCCESS);

}
