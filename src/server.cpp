#include "server.h"
#include <iostream>

using namespace RikerIO;

Server::Server(jsonrpc::UnixDomainSocketServer& server, std::string& id) :
    AbstractStubServer(server),
    id(id),
    masterFactory() {



}

Json::Value Server::master_register(const std::string& name, int pid) {

    printf("master.register called with args %s and %d.\n", name.c_str(), pid);

    Json::Value result;

    try {
        Master& master = masterFactory.create(name, pid);

        printf("master.register created new master with token %s and id %d.\n", master.getToken().c_str(), master.getId());

        result["code"] = 0;
        result["data"] = master.getToken(); // token

        return result;
    } catch (std::runtime_error& e) {

        printf("master.register failed, master could not be created.\n");

        result["code"] = 1;
        result["data"] = "";

        return result;

    }

}
Json::Value Server::master_unregister(const std::string& token) {

    printf("master.unregister called with token %s\n", token.c_str());

    Json::Value result;

    if (masterFactory.remove(token)) {

        printf("master.unregister successfull.\n");

        result["code"] = 0;

        return result;

    }

    printf("master.unregister unsuccessfull.\n");

    result["code"] = 1;
    return result;

}


Json::Value Server::master_list() {

    printf("master.list called\n");

    std::vector<std::shared_ptr<RikerIO::Master>> list = masterFactory.list();

    Json::Value result;
    Json::Value resList = Json::arrayValue;


    for (auto m : list) {

        Json::Value mObj(m->getLabel());

        resList.append(mObj);

    }

    result["code"] = 0;
    result["data"] = resList;

    printf("master.list returning list of %ld master labels.\n", list.size());

    return result;

}


Json::Value Server::alloc(int size, const std::string& token) { }
Json::Value Server::dealloc(int offset, const std::string& token) { }
Json::Value Server::data_create(const Json::Value& data, const std::string& id, const std::string& token) { }
Json::Value Server::data_remove(const std::string& id, const std::string& token) { }
Json::Value Server::data_list(const std::string& id) { }
Json::Value Server::data_get(const std::string& id) { }
Json::Value Server::link_add(const std::string& dataId, const std::string& linkId) { }
Json::Value Server::link_remove(const std::string& dataId, const std::string& linkId) { }
Json::Value Server::link_list(const std::string& pattern) { }
Json::Value Server::link_get(const std::string& id) { }
