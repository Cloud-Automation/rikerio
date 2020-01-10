
#ifndef __RIKERIO_JSON_SERVER_H_
#define __RIKERIO_JSON_SERVER_H_

#include "spdlog/spdlog.h"
#include "jsonrpccpp/server.h"
#include "algorithm"
#include "common/error.h"
#include "common/type.h"
#include "common/mem-position.h"
#include "server/data.h"
#include "server/memory.h"
#include "server/server-interface.h"

class JsonServer : public jsonrpc::AbstractServer<JsonServer> {
  public:
    JsonServer(
        jsonrpc::AbstractServerConnector &conn,
        RikerIO::ServerInterface& server,
        jsonrpc::serverVersion_t type = jsonrpc::JSONRPC_SERVER_V2);

    inline void configI(const Json::Value& request, Json::Value& response);
    inline void memory_allocI(const Json::Value &request, Json::Value &response);
    inline void memory_deallocI(const Json::Value &request, Json::Value &response);
    inline void memory_listI(const Json::Value & request, Json::Value &response);
    inline void memory_getI(const Json::Value& request, Json::Value &response);
    inline void data_addI(const Json::Value &request, Json::Value &response);
    inline void data_removeI(const Json::Value &request, Json::Value &response);
    inline void data_listI(const Json::Value &request, Json::Value &response);
    inline void link_addI(const Json::Value &request, Json::Value &response);
    inline void link_removeI(const Json::Value &request, Json::Value &response);
    inline void link_listI(const Json::Value &request, Json::Value &response);

  private:
    RikerIO::ServerInterface& server;

};

#endif //JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_
