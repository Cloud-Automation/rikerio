#ifndef __RIO_SERVER_H__
#define __RIO_SERVER_H__

#include "server/abstractstubserver.h"
#include "common/task.h"
#include "common/owner.h"
#include "common/memory.h"
#include "common/data.h"

#include <jsonrpccpp/server/connectors/unixdomainsocketserver.h>
#include <string>

namespace RikerIO {

class Server : public AbstractStubServer {

  public:
    Server(jsonrpc::UnixDomainSocketServer&, std::string&);

    Json::Value task_register(const std::string& name, int pid, bool track);
    Json::Value task_unregister(const std::string& token);
    Json::Value task_list();
    Json::Value memory_alloc(int size, const std::string& token);
    Json::Value memory_dealloc(int offset, const std::string& token);
    Json::Value memory_inspect();
    Json::Value data_create(const Json::Value& data, const std::string& id, const std::string& token);
    Json::Value data_remove(const std::string& id, const std::string& token);
    Json::Value data_list(const std::string& id);
    Json::Value data_get(const std::string& id);
    Json::Value link_add(const std::string& dataId, const std::string& linkId);
    Json::Value link_remove(const std::string& dataId, const std::string& linkId);
    Json::Value link_list(const std::string& pattern);
    Json::Value link_get(const std::string& id);
    Json::Value link_updates(const std::string& token);

  private:

    std::string& id;

    TaskFactory taskFactory;

    /* memory management */
    Memory memory;
    OwnerFactory<unsigned int, unsigned int> allocOwnerFactory;

    /* data management */
    DataFactory dataFactory;
    OwnerFactory<std::string, unsigned int> dataOwnerFactory;

};

}


#endif
