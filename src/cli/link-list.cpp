#include "client/client.h"
#include "jsonrpccpp/client/connectors/unixdomainsocketclient.h"
#include <iostream>
#include <algorithm>

bool cmd_link_list_comp_offset(RikerIO::AbstractClient::LinkListItem& a, RikerIO::AbstractClient::LinkListItem& b) {

    if (!a.data || !b.data) {
        return false;
    }

    if (a.data->offset == b.data->offset) {
        return a.data->index < b.data->index;
    }

    return a.data->offset < b.data->offset;

}

bool cmd_link_list_comp_key(RikerIO::AbstractClient::LinkListItem& a, RikerIO::AbstractClient::LinkListItem& b) {

    return a.key < b.key;

}

bool cmd_link_list_comp_id(RikerIO::AbstractClient::LinkListItem& a, RikerIO::AbstractClient::LinkListItem& b) {

    return a.id < b.id;

}

void cmd_link_list(
    RikerIO::Client& client,
    const std::string& pattern,
    const std::string& sortBy,
    bool extendedList,
    bool descending,
    bool hideEmptyLinks) {


    RikerIO::AbstractClient::LinkListResponse response;

    client.link_list(pattern, response);

    if (sortBy == "offset") {
        sort(response.list.begin(), response.list.end(), cmd_link_list_comp_offset);
    } else if (sortBy == "id") {
        sort(response.list.begin(), response.list.end(), cmd_link_list_comp_id);
    } else if (sortBy == "key") {
        sort(response.list.begin(), response.list.end(), cmd_link_list_comp_key);
    }

    if (descending) {
        std::reverse(response.list.begin(), response.list.end());
    }

    if (extendedList) {

        unsigned int maxLength = 0;
        for (auto a : response.list) {
            maxLength = std::max(maxLength, (unsigned int)a.id.length());
        }

        std::string idHeader = "ID";
        for (unsigned int i = 0; i < maxLength; i+= 1) {
            idHeader += " ";
        }

        printf("%sOFFSET  SIZE    SEMAPHORE FLAGS TYPE       KEY\n", idHeader.c_str());

        for (auto a : response.list) {

            std::string altId = a.id;

            for (unsigned int i = 0; i < maxLength - a.id.length() + 2; i+= 1) {
                altId += " ";
            }

            if (!a.data) {

                if (!hideEmptyLinks) {
                    printf("%s-       -       -         -     -%-10s%s\n",
                           altId.c_str(),
                           "",
                           a.key.c_str());
                }

                continue;
            }

            std::string sOffset = std::to_string(a.data->offset) + "." + std::to_string(a.data->index);
            std::string flags = "";
            flags += a.data->isPrivate ? "P" : "-";

            printf("%s%-8s%-8d%-10d%-6s%-11s%s\n",
                   altId.c_str(),
                   sOffset.c_str(),
                   a.data->size,
                   a.data->semaphore,
                   flags.c_str(),
                   a.data->datatype.c_str(),
                   a.key.c_str());
        }

    } else {
        for (auto a : response.list) {
            printf("%s;%s\n",
                   a.key.c_str(),
                   a.id.c_str());
        }


    }

    exit(EXIT_SUCCESS);

}
