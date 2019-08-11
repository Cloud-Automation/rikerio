#include "server/server.h"
#include "common/error.h"
#include <iostream>

using namespace RikerIO;

Server::Server(jsonrpc::UnixDomainSocketServer& server, std::string& id) :
    AbstractStubServer(server),
    id(id),
    taskFactory(),
    memory(4096, id),
    allocOwnerFactory(),
    dataFactory(),
    dataOwnerFactory() {



}

Json::Value Server::task_register(const std::string& name, int pid, bool track) {

    printf("task.register called with args %s, %d and %d.\n", name.c_str(), pid, track);

    Json::Value result;

    try {

        Task& task = taskFactory.create(name, pid, track);

        printf("task.register created new master with token %s and id %d.\n", task.getToken().c_str(), task.getId());

        result["code"] = RikerIO::NO_ERROR;
        result["data"] = task.getToken(); // token

    } catch (RikerIO::GenerateTokenException& e) {

        printf("task.register failed, master token could not be created.\n");

        result["code"] = GENTOKEN_ERROR;
        result["data"] = "";


    }

    return result;

}
Json::Value Server::task_unregister(const std::string& token) {

    printf("task.unregister called with token %s\n", token.c_str());

    Json::Value result;

    /* ToDo : remove data associated with this task */

    /* ToDo : remove allocations associated with this task */

    if (!taskFactory.remove(token)) {
        printf("task.unregister not unsuccessfull.\n");

        result["code"] = RikerIO::Error::NOTFOUND_ERROR;
        return result;
    }

    printf("task.unregister successfull.\n");

    result["code"] = RikerIO::Error::NO_ERROR;

    return result;


}


Json::Value Server::task_list() {

    printf("task.list called\n");

    std::vector<std::shared_ptr<RikerIO::Task>> list = taskFactory.list();

    Json::Value result;
    Json::Value resList = Json::arrayValue;


    for (auto m : list) {

        Json::Value mObj(m->getLabel());

        resList.append(mObj);

    }

    result["code"] = 0;
    result["data"] = resList;

    printf("task.list returning list of %ld master labels.\n", list.size());

    return result;

}


Json::Value Server::memory_alloc(int size, const std::string& token) { }
Json::Value Server::memory_dealloc(int offset, const std::string& token) { }
Json::Value Server::memory_inspect() { }

Json::Value Server::data_create(const Json::Value& data, const std::string& id, const std::string& token) {

    Json::Value result;

    std::shared_ptr<Task> task = taskFactory[token];

    if (!task) {
        result["code"] = RikerIO::Error::UNAUTHORIZED_ERROR;
        return result;
    }

    /* create ownership first */

    if (!dataOwnerFactory.assign(id, task->getId())) {
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

    std::shared_ptr<Task> task = taskFactory[token];

    if (!task) {
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

    if (ownerId != task->getId()) {
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
Json::Value Server::link_updates(const std::string& token) { }
