#include "client/response/config-get.h"

RikerIO::Response::v1::ConfigGet::ConfigGet(Json::Value& result) : RPCResponse(result) {

    if (!ok()) {
        return;
    }

    Json::Value data = result["data"];

    profile = data["id"].asString();
    version = data["version"].asString();
    shm_file = data["shm"].asString();
    size = data["size"].asUInt();
    cycle = data["cycle"].asUInt();

}

const std::string& RikerIO::Response::v1::ConfigGet::get_profile() {
    return profile;
}

const std::string& RikerIO::Response::v1::ConfigGet::get_version () {
    return version;
}

const std::string& RikerIO::Response::v1::ConfigGet::get_shm_file () {
    return shm_file;
}

unsigned int RikerIO::Response::v1::ConfigGet::get_size () {
    return size;
}

unsigned int RikerIO::Response::v1::ConfigGet::get_cycle () {
    return cycle;
}
