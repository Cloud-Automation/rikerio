#include "client/client.h"
#include <algorithm>

bool cmd_link_list_comp_offset(
    std::shared_ptr<RikerIO::Response::v1::LinkList::LinkListItem> a,
    std::shared_ptr<RikerIO::Response::v1::LinkList::LinkListItem> b) {

    if (!a->get_data() || !b->get_data()) {
        return false;
    }

    if (a->get_data()->get_offset() == b->get_data()->get_offset()) {
        return a->get_data()->get_index() < b->get_data()->get_index();
    }

    return a->get_data()->get_offset() < b->get_data()->get_offset();

}

bool cmd_link_list_comp_key(
    std::shared_ptr<RikerIO::Response::v1::LinkList::LinkListItem> a,
    std::shared_ptr<RikerIO::Response::v1::LinkList::LinkListItem> b) {

    return a->get_key() < b->get_key();

}

bool cmd_link_list_comp_id(
    std::shared_ptr<RikerIO::Response::v1::LinkList::LinkListItem> a,
    std::shared_ptr<RikerIO::Response::v1::LinkList::LinkListItem> b) {

    return a->get_id() < b->get_id();

}

std::shared_ptr<RikerIO::RPCResponse> cmd_link_list(
    RikerIO::Client& client,
    const std::string& pattern,
    const std::string& sortBy,
    bool extendedList,
    bool descending,
    bool hideEmptyLinks) {


    RikerIO::Request::v1::LinkList req(pattern);

    auto response = client.link_list(req);

    auto items = response->get_items();

    if (sortBy == "offset") {
        sort(items.begin(), items.end(), cmd_link_list_comp_offset);
    } else if (sortBy == "id") {
        sort(items.begin(), items.end(), cmd_link_list_comp_id);
    } else if (sortBy == "key") {
        sort(items.begin(), items.end(), cmd_link_list_comp_key);
    }

    if (descending) {
        std::reverse(items.begin(), items.end());
    }

    if (extendedList) {

        unsigned int maxLength = 0;
        for (auto a : items) {
            maxLength = std::max(maxLength, (unsigned int)a->get_id().length());
        }

        std::string idHeader = "ID";
        for (unsigned int i = 0; i < maxLength; i+= 1) {
            idHeader += " ";
        }

        printf("%sOFFSET  SIZE    SEMAPHORE FLAGS TYPE       KEY\n", idHeader.c_str());

        for (auto a : items) {

            std::string altId = a->get_id();

            for (unsigned int i = 0; i < maxLength - a->get_id().length() + 2; i+= 1) {
                altId += " ";
            }

            if (!a->get_data()) {

                if (!hideEmptyLinks) {
                    printf("%s-       -       -         -     -%-10s%s\n",
                           altId.c_str(),
                           "",
                           a->get_key().c_str());
                }

                continue;
            }

            std::string sOffset = std::to_string(a->get_data()->get_offset()) + "." + std::to_string(a->get_data()->get_index());
            std::string flags = "";
            flags += a->get_data()->is_private() ? "P" : "-";

            printf("%s%-8s%-8d%-10d%-6s%-11s%s\n",
                   altId.c_str(),
                   sOffset.c_str(),
                   a->get_data()->get_size(),
                   a->get_data()->get_semaphore(),
                   flags.c_str(),
                   RikerIO::Utils::GetStringFromType(a->get_data()->get_datatype()).c_str(),
                   a->get_key().c_str());
        }

    } else {
        for (auto a : items) {
            printf("%s;%s\n",
                   a->get_key().c_str(),
                   a->get_id().c_str());
        }


    }

    return response;

}
