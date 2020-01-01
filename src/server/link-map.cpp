#include "server/link-map.h"
#include <iostream>
#include <fstream>

RikerIO::LinkMap::LinkMap(const std::string filename) : std::multimap<const std::string, const std::string>(), filename(filename) {}

bool RikerIO::LinkMap::exist(const std::string& linkname, const std::string& data_id) const {
    typedef LinkMap::const_iterator it;

    const std::pair<std::string, std::string> pair = std::make_pair(linkname, data_id);
    std::pair<it,it> range = equal_range(pair.first);
    for (it p = range.first; p != range.second; ++p)
        if (p->second == pair.second)
            return true;

    return false;

}

bool RikerIO::LinkMap::add(const std::string& linkname, const std::string& data_id) {

    if (exist(linkname, data_id)) {
        return false;
    }

    std::pair<const std::string, const std::string> pair = std::make_pair(linkname, data_id);
    insert(pair);

    return true;

}

std::set<std::string> RikerIO::LinkMap::keys() const {

    std::set<std::string> result;

    for (auto it = begin(); it != end(); it = upper_bound(it->first)) {
        //printf("%s\n", it->first.c_str());
        result.insert((*it).first);
    }

    return result;

}

unsigned int RikerIO::LinkMap::remove(const std::string& linkname) {

    return erase(linkname);

}

bool RikerIO::LinkMap::remove(const std::string& linkname, const std::string& data_id) {
    typedef LinkMap::const_iterator it;

    const std::pair<std::string, std::string> pair = std::make_pair(linkname, data_id);
    std::pair<it,it> range = equal_range(pair.first);
    for (it p = range.first; p != range.second; ++p) {
        if (p->second == pair.second) {
            erase(p);
            return true;
        }
    }

    return false;

}

void RikerIO::LinkMap::iterate(std::function<void(const std::string& key, const std::string& data_id)> handler) {

    for (auto it = begin(); it != end(); ++it) {
        handler((*it).first, (*it).second);
    }

}

void RikerIO::LinkMap::serialize() {

    std::ofstream file(filename);

    for (auto it = begin(); it != end(); ++it) {
        file << (*it).first << ";" << (*it).second << std::endl;
    }

    file.close();

}

void RikerIO::LinkMap::deserialize() {

    std::ifstream file(filename);

    if (!file.is_open()) {
        return;
    }

    clear();

    std::string line;
    while (getline(file, line)) {

        size_t pos = line.find(";");

        if (pos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string data = line.substr(pos + 1, line.length());

        add(key, data);

    }

    file.close();

}
