#ifndef __RIKERIO_SERVER_DATA_LIST_H__
#define __RIKERIO_SERVER_DATA_LIST_H__

#include "vector"
#include "server/data-add.h"

namespace RikerIO {

struct DataListRequest {
    const std::string pattern;
    DataListRequest(std::string pattern) : pattern(pattern) { }
};

struct DataListResponseItem {

    std::string id;
    Type type;
    MemoryPosition offset;
    int semaphore;
    bool is_private;

    DataListResponseItem() :
        id(""),
        type(),
        offset(),
        semaphore(-1),
        is_private(false) { }

};

struct DataListResponse {
    std::vector<DataListResponseItem> list;
    DataListResponse() : list() { }
};


}

#endif
