#ifndef __RIO_SERVER_H__
#define __RIO_SERVER_H__

#include "server/abstractstubserver.h"

#include "server/memory.h"
#include "server/data.h"
#include "server/data-map.h"
#include "server/link-map.h"

#include <jsonrpccpp/server/connectors/unixdomainsocketserver.h>
#include <string>

#define FOLDER "/var/lib/rikerio"
#define LINKS_FILENAME "links"
#define SHM_FILENAME "shm"


namespace RikerIO {

class Server : public AbstractStubServer {

  public:

    Server(
        jsonrpc::UnixDomainSocketServer&,
        const std::string&,
        unsigned int,
        unsigned int);

    ~Server();

    void get_config(ConfigResponse&);

    void memory_alloc(int, MemoryAllocResponse&);
    void memory_dealloc(const std::string& token);
    void memory_list(MemoryListResponse&);
    void memory_get(int offset, MemoryGetResponse&);

    /**
     * @brief
     * Here are the rules for creating data points
     *
     * 1. if you own a memory token and use it when creating a data point, the data point
     *    is bound to the memory area. Is it deallocated then the data point will be removed.
     *    The data point is considered private since the memory token owner is the only one
     *    who can remove the data point. But it is still readable by all others.
     * 2. if you create a data point with an empty token (character count zero) then this data point
     *    is public meaning everyone can remove it. It musst be located inside a allocated memory area
     *    and when that memory area is deallocated, this data point will be removed.
     * 3. Data points with a size bigger than 8 bit cannot have an index. Index and size information musst
     *    contain the data inside the size of one byte. Meaning a data point with a bitsize of 5 cannot be
     *    at index > 3.
     * 4. Data points cannot be declared with a type and size information at the same time.
     * 5. Data points with unknown datatype can be of any size.
     * 6. Consider the memory boundaries.
     *
     */

    void data_create(
        const std::string& token,
        const std::string& dId,
        const DataCreateRequest&);

    void data_remove(
        const std::string& pattern,
        const std::string& token,
        DataRemoveResponse& res);

    void data_list(
        const std::string& filterPattern,
        DataListResponse& response);

    Json::Value data_get(const std::string& dId);

    void link_add(
        const std::string& linkname,
        std::vector<std::string>& dataIds,
        unsigned int&);
    void link_remove(const std::string& linkname, std::vector<std::string>& data_ids, unsigned int& counter);
    void link_list(const std::string& pattern, AbstractStubServer::LinkListResponse&);

    Json::Value link_get(const std::string& lId);

    std::set<std::shared_ptr<MemoryArea>> getMemoryAreas();

  private:

    const std::string& id;
    unsigned int size;
    unsigned int cycle;

    static std::mutex alloc_mutex;
    static std::mutex persistent_mutex;

    /* memory management */
    Memory memory;

    /*    std::map<const std::string, std::shared_ptr<Data>> dataMap;
        std::map<const std::string, std::string> dataTokenMap;
        std::map<const std::string, std::shared_ptr<MemoryArea>> dataMemoryMap;
    */
    DataMap dataMap;
    LinkMap linkMap;

    static bool match(const std::string& pattern, const std::string& target);

    std::atomic<bool> persistentThreadRunning;
    std::atomic<unsigned int> persistentChangeCount;
    std::shared_ptr<std::thread> persistentThread;
    void makeLinksPersistent();

};

}


#endif
