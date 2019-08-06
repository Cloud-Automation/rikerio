#include "server/server.h"
#include "common/error.h"
#include <iostream>

using namespace RikerIO;

Server::Server(jsonrpc::UnixDomainSocketServer& server, std::string& id) :
    AbstractStubServer(server),
    id(id),
    masterFactory(),
    memory(4096, id),
    allocOwnerFactory(),
    dataFactory(),
    dataOwnerFactory() {



}

Json::Value Server::master_register(const std::string& name, int pid) {

    printf("master.register called with args %s and %d.\n", name.c_str(), pid);

    Json::Value result;

    try {

        Master& master = masterFactory.create(name, pid);

        printf("master.register created new master with token %s and id %d.\n", master.getToken().c_str(), master.getId());

        result["code"] = RikerIO::NO_ERROR;
        result["data"] = master.getToken(); // token

    } catch (RikerIO::GenerateTokenException& e) {

        printf("master.register failed, master token could not be created.\n");

        result["code"] = GENTOKEN_ERROR;
        result["data"] = "";


    }

    return result;

}
Json::Value Server::master_unregister(const std::string& token) {

    printf("master.unregister called with token %s\n", token.c_str());

    Json::Value result;

    if (masterFactory.remove(token)) {

        printf("master.unregister successfull.\n");

        result["code"] = RikerIO::Error::NO_ERROR;

        return result;

    }

    printf("master.unregister not unsuccessfull.\n");

    result["code"] = RikerIO::Error::NOTFOUND_ERROR;
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

Json::Value Server::data_create(const Json::Value& data, const std::string& id, const std::string& token) {

    Json::Value result;

    std::shared_ptr<Master> master = masterFactory[token];

    if (!master) {
        result["code"] = RikerIO::Error::UNAUTHORIZED_ERROR;
        return result;
    }

    /* create ownership first */

    if (!dataOwnerFactory.assign(id, master->getId())) {
        result["code"] = RikerIO::Error::UNAUTHORIZED_ERROR;
        return result;
    }

    /* create database entry */

    RikerIO::Data d(
        RikerIO::Data::GetTypeFromString(data["datatype"].asString()),
        data["byteOffset"].asUInt(),
        data["bitOffset"].asUInt(),
        data["bitSize"].asUInt());

    if (dataFactory.create(id, d)) {
        result["code"] = RikerIO::Error::NO_ERROR;
        return result;
    }

    result["code"] = RikerIO::Error::DUPLICATE_ERROR;
    return result;

}

Json::Value Server::data_remove(const std::string& id, const std::string& token) {

    Json::Value result;

    std::shared_ptr<Master> master = masterFactory[token];

    if (!master) {
        result["code"] = RikerIO::Error::UNAUTHORIZED_ERROR;
        return result;
    }

    /* chcek ownership */

    unsigned int ownerId = 0;
    try {
        ownerId = dataOwnerFactory.getOwner(id);
    } catch (RikerIO::NotFoundException& e) {
        result["code"] = RikerIO::Error::NOTFOUND_ERROR;
        return result;
    }

    if (ownerId != master->getId()) {
        result["code"] = RikerIO::Error::UNAUTHORIZED_ERROR;
        return result;
    }

    /* remove ownership */

    if (!dataOwnerFactory.remove(id)) {
        printf("Error removing owner of data %s, that should not happen.\n", id.c_str());
    }

    /* remove data */

    if (!dataFactory.remove(id)) {
        printf("Error removing data %s, that should not happen.\n", id.c_str());
    }

    result["code"] = RikerIO::Error::NO_ERROR;
    return result;



}

Json::Value Server::data_list(const std::string& pattern) {

    Json::Value result;

    std::vector<std::string> list = dataFactory.filter(pattern);

    result["code"] = 0;
    result["data"] = Json::arrayValue;

    unsigned int index = 0;
    for (auto l : list) {
        result["data"][index++] = l;
    }

    return result;

}


Json::Value Server::data_get(const std::string& id) { }
Json::Value Server::link_add(const std::string& dataId, const std::string& linkId) { }
Json::Value Server::link_remove(const std::string& dataId, const std::string& linkId) { }
Json::Value Server::link_list(const std::string& pattern) { }
Json::Value Server::link_get(const std::string& id) { }
