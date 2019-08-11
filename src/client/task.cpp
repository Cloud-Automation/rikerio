#include "client/task.h"
#include "client/allocation.h"

using namespace RikerIO::Client;

unsigned int Task::localTaskCounter = 0;

Task::Task(AbstractClient& client, std::string token) :
    id(Task::localTaskCounter++),
    client(client),
    token(token),
    allocMap() {

}

Task::~Task() {

}

unsigned int Task::getId() {
    return id;
}

RikerIO::AbstractClient& Task::getClient() {
    return client;
}

std::string Task::getToken() {
    return token;
}

Allocation& Task::alloc(unsigned int size) {

    unsigned int offset = client.memory_alloc(size, token);

    auto alc = std::make_shared<Allocation>(offset, size);

    allocMap[offset] = alc;
    return *alc;

}

void Task::dealloc(Allocation& a) {

    client.memory_dealloc(a.getOffset(), token);

    allocMap.erase(a.getOffset());

}

void Task::registerData(RikerIO::Data& data, std::string id) {

    client.data_create(data, id, token);

}
