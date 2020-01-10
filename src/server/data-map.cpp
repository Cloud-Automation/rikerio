#include "server/data-map.h"

RikerIO::DataMap::DataMap(Memory& memory) : memory(memory) { }

bool RikerIO::DataMap::add(const std::string id, std::shared_ptr<Data> data) {

    MemoryAreaPtr memArea = memory.get_area_from_range(
                                data->get_offset().get_byte_offset(),
                                data->get_type().get_byte_size());

    // no memory area for this data point found
    if (!memArea) {
        return false;
    }

    // found token for this id, cannot go further.
    if (dataTokenMap.find(id) != dataTokenMap.end()) {
        return false;
    }

    (*this)[id] = data;
    dataMemoryMap[id] = memArea;

    return true;

}

bool RikerIO::DataMap::add(const std::string id, const std::string token, std::shared_ptr<Data> data) {

    MemoryAreaPtr memArea = memory.get_area_from_token(token);

    // no memory area for this token
    if (!memArea) {
        return false;
    }

    MemoryAreaPtr realMemArea =
        memory.get_area_from_range(
            data->get_offset().get_byte_offset(),
            data->get_type().get_byte_size());

    if (memArea != realMemArea) {
        return false;
    }

    if (memArea->get_token() != token) {
        return false;
    }

    // found token for this id, cannot go further.
    if (dataTokenMap.find(id) != dataTokenMap.end()) {
        if (dataTokenMap[id] != token) {
            return false;
        }
    }

    (*this)[id] = data;
    dataTokenMap[id] = token;
    dataMemoryMap[id] = memArea;

    return true;

}

bool RikerIO::DataMap::remove(const std::string& id) {

    if (dataTokenMap.find(id) != dataTokenMap.end()) {
        return false;
    }

    if (find(id) == end()) {
        return false;
    }

    erase(id);
    dataMemoryMap.erase(id);

    return true;

}

bool RikerIO::DataMap::remove(const std::string& id, const std::string& token) {

    if (dataTokenMap.find(id) == dataTokenMap.end()) {
        return false;
    }

    if (dataTokenMap[id] != token) {
        return false;
    }

    if (find(id) == end()) {
        return false;
    }

    erase(id);
    dataTokenMap.erase(id);
    dataMemoryMap.erase(id);

    return true;

}

unsigned int RikerIO::DataMap::remove_by_token(const std::string& token) {

    std::set<std::string> removeList;

    for (auto dt : dataTokenMap) {

        if (dt.second == token) {
            removeList.insert(dt.first);
        }

    }

    for (auto rli : removeList) {

        erase(rli);
        dataTokenMap.erase(rli);
        dataMemoryMap.erase(rli);

    }

    return removeList.size();

}

unsigned int RikerIO::DataMap::remove_by_range(unsigned int offset, unsigned int size) {

    std::set<std::string> removeList;

    for (auto t : (*this)) {

        if (t.second->inRange(offset, size)) {
            removeList.insert(t.first);
        }

    }

    for (auto rli : removeList) {

        erase(rli);
        dataTokenMap.erase(rli);
        dataMemoryMap.erase(rli);

    }

    return removeList.size();

}

bool RikerIO::DataMap::is_private(const std::string& id) {

    return dataTokenMap.find(id) != dataTokenMap.end();

}

int RikerIO::DataMap::get_semaphore(const std::string& id) {

    if (dataMemoryMap.find(id) == dataMemoryMap.end()) {
        return -1;
    }

    return dataMemoryMap[id]->get_semaphore();

}
