#ifndef __RIKERIO_SERVER_LINK_LIST_H__
#define __RIKERIO_SERVER_LINK_LIST_H__

#include "string"
#include "vector"
#include "server/data-list.h"

namespace RikerIO {

struct LinkListRequest {
    const std::string pattern;
    LinkListRequest(std::string pattern) : pattern(pattern) { }
};

struct LinkListItem {
    std::string key;
    std::string id;
    bool has_data;
    DataListResponseItem data_item;
    LinkListItem(std::string key, std::string id, DataListResponseItem* data_item = NULL) :
        key(key), id(id), has_data(data_item != NULL), data_item(*data_item) { }
};

struct LinkListResponse {
    std::vector<LinkListItem> list;
    LinkListResponse () : list() { }
};


}


#endif
