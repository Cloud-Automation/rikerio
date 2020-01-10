#ifndef __RIO_SERVER_H__
#define __RIO_SERVER_H__

#include "server/server-interface.h"
#include "server/memory.h"
#include "server/data.h"
#include "server/data-map.h"
#include "server/link-map.h"
#include "thread"
#include "atomic"
#include "mutex"
#include "string"


namespace RikerIO {

class Server : public ServerInterface {

  public:

    Server(
        const std::string&,
        unsigned int,
        unsigned int);

    ~Server();

    void config_get(ConfigResponse&);
    void memory_alloc(MemoryAllocRequest&, MemoryAllocResponse&);
    void memory_dealloc(MemoryDeallocRequest&);
    void memory_list(MemoryListResponse&);
    void memory_get(MemoryGetRequest&, MemoryGetResponse&);
    void data_add(DataAddRequest&, DataAddResponse&);
    void data_remove(DataRemoveRequest&, DataRemoveResponse&);
    void data_list(DataListRequest&, DataListResponse&);
    void link_add(LinkAddRequest&, LinkAddResponse&);
    void link_remove(LinkRemoveRequest&, LinkRemoveResponse&);
    void link_list(LinkListRequest&, LinkListResponse&);

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
