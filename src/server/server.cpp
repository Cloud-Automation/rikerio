
#include "server/server.h"
#include "spdlog/spdlog.h"
#include "common/error.h"
#include "common/config.h"
#include "version.h"
#include "fnmatch.h"


using namespace RikerIO;

std::mutex Server::alloc_mutex;
std::mutex Server::persistent_mutex;

Server::Server(
    jsonrpc::UnixDomainSocketServer& server,
    const std::string& id,
    unsigned int size,
    unsigned int cycle) :
    AbstractStubServer(server),
    id(id),
    size(size),
    cycle(cycle),
    memory(size, RikerIO::Config::CreateShmPath(id)),
    dataMap(memory),
    linkMap(RikerIO::Config::CreateLinkPath(id)) {


    linkMap.deserialize();

    persistentChangeCount = 0;
    persistentThreadRunning = true;
    persistentThread = std::make_shared<std::thread>(std::bind(&Server::makeLinksPersistent, this));

}

Server::~Server() {

    persistentThreadRunning = false;
    persistentThread->join();

}

void Server::get_config(ConfigResponse& response) {

    response.profile = id;
    response.version = std::string(RIO_VERSION_STRING);
    response.shmFile = std::string(FOLDER) + "/" + id + "/" + SHM_FILENAME;
    response.size = size;
    response.defaultCycle = cycle;

}

void Server::memory_alloc(int size, MemoryAllocResponse& response) {

    const std::lock_guard<std::mutex> lock(Server::alloc_mutex);

    if (size <= 0) {
        throw ServerError(BAD_REQUEST, "Size musst be > 0.");
    }

    try  {

        std::shared_ptr<MemoryArea> ma = memory.alloc(size);

        response.offset = ma->getOffset();
        response.token = ma->getToken();
        response.semaphore = ma->getSemaphore();

    } catch (Token::TokenException& e) {
        throw ServerError(GENTOKEN_ERROR, "Internal Error (1).");
    } catch (OutOfSpaceError& e) {
        throw ServerError(OUTOFSPACE_ERROR, "Not enough memory for allocation.");
    }

}

void Server::memory_dealloc(const std::string& token) {

    /* get offset by token */

    if (memory.getAreaFromToken(token) == nullptr) {
        throw ServerError(UNAUTHORIZED_ERROR, "Token not found.");
    }

    std::shared_ptr<MemoryArea> maFromToken = memory.getAreaFromToken(token);
    std::shared_ptr<MemoryArea> maFromDealloc = memory.dealloc(maFromToken->getOffset());

    if (maFromDealloc == nullptr) {
        throw ServerError(INTERNAL_ERROR, "Internal Error (1).");
    }

    dataMap.removeByRange(maFromToken->getOffset(), maFromToken->getSize());

}

void Server::memory_list(MemoryListResponse& response) {

    for (auto ma : memory) {
        response.data.push_back(ma);
    }

}

void Server::memory_get(int offset, MemoryGetResponse& response) {

    if (offset < 0) {
        throw ServerError(BAD_REQUEST, "Offset cannot be negative.");
    }

    for (auto ma : memory) {

        if (ma->getOffset() != static_cast<unsigned int>(offset)) {
            continue;
        }

        response.data = ma;

        return;

    }

    throw ServerError(NOTFOUND_ERROR, "Memory Area not found.");

}

void Server::data_add(
    const std::string& id,
    DataAddRequest& req,
    DataAddResponse& res) {

    spdlog::debug("data_add({},{},{},{})",id, req.token, req.type.to_string(), req.offset.to_string());

    std::shared_ptr<MemoryArea> memArea = nullptr;
    std::shared_ptr<Data> entry = nullptr;

    bool hasToken = req.token.length() > 0;

    if (!Data::isValidId(id)) {
        throw ServerError(BAD_REQUEST, "Data ID is not valid.");
    }

    /* get offset by token */

    if (hasToken) {

        memArea = memory.getAreaFromToken(req.token);

        if (!memArea) {
            throw ServerError(UNAUTHORIZED_ERROR, "Token not found.");
        }

        spdlog::debug("found a valid token.");

    } else {

        memArea = memory.getAreaFromRange(req.offset.get_byte_offset(), req.type.get_byte_size());

    }

    if (!memArea) {
        throw ServerError(BAD_REQUEST, "Data not inside memory area.");
    }

    spdlog::debug("found a valid memory area.");

    if (hasToken) {
        spdlog::debug("adding offset.");
        req.offset.add_byte_offset(memArea->getOffset());
    }

    std::shared_ptr<RikerIO::Data> d = std::make_shared<RikerIO::Data>(req.type, req.offset);

    bool value = false;
    if (hasToken) {
        value = dataMap.add(id, req.token, d);
    } else {
        value = dataMap.add(id, d);
    }

    spdlog::debug("Adding data result {}.", value);

    res.type = req.type;
    res.offset = req.offset;
    res.semaphore = memArea->getSemaphore();

}

void Server::data_remove(const std::string& pattern, const std::string& token, DataRemoveResponse& response) {

    unsigned int count = 0;

    for (auto d : dataMap) {

        if (!Server::match(pattern, d.first)) {
            continue;
        }

        bool removeSuccessfull = false;

        if (token.length() == 0) {
            removeSuccessfull = dataMap.remove(d.first);
        } else {
            removeSuccessfull = dataMap.remove(d.first, token);
        }

        /*        std::string dataToken = "";

                if (dataTokenMap.find(d.first) != dataTokenMap.end()) {
                    dataToken = dataTokenMap[d.first];
                }

                if (dataToken != "" && dataToken != token) {
                    continue;
                }
        */
        count += removeSuccessfull ? 1 : 0;

        /*        dataMap.erase(d.first);
                dataTokenMap.erase(d.first);
                dataMemoryMap.erase(d.first);
        */
    }

    response.count = count;

}


void Server::data_list(const std::string& pattern, DataListResponse& response) {

    for (auto d : dataMap) {

        if (!Server::match(pattern, d.first)) {
            continue;
        }


        std::shared_ptr<RikerIO::Data> data = d.second;

        bool isPrivate = dataMap.isPrivate(d.first);

        DataListResponseItem item = {
            d.first,
            data->get_offset(),
            data->get_type(),
            dataMap.getSemaphore(d.first),
            isPrivate
        };

        response.list.push_back(item);

    }

}


Json::Value Server::data_get(const std::string& dId) {

    (void)(dId);

    Json::Value result;
    result["code"] = 0;

    return result;

}


void Server::link_add(const std::string& linkname, std::vector<std::string>& data_ids, unsigned int& counter) {

    const std::lock_guard<std::mutex> lock(persistent_mutex);

    counter = 0;

    for (std::string s : data_ids) {
        if (linkMap.add(linkname, s)) {
            counter += 1;
        }
    }

    /* start persistent thread */

    persistentChangeCount += counter;

}

void Server::link_remove(const std::string& pattern, std::vector<std::string>& data_ids, unsigned int& counter) {

    const std::lock_guard<std::mutex> lock(persistent_mutex);

    counter = 0;

    std::set<std::string> keys = linkMap.keys();

    for (auto key : keys) {

        if (!Server::match(pattern, key)) {

            continue;

        }

        if (data_ids.size() == 0) {

            counter += linkMap.remove(key);

        } else {

            for (auto data : data_ids) {
                counter += linkMap.remove(key, data) ? 1 : 0;
            }

        }
    }

    persistentChangeCount += counter;

}


void Server::link_list(const std::string& pattern, AbstractStubServer::LinkListResponse& response) {

    const std::lock_guard<std::mutex> lock(persistent_mutex);

    linkMap.iterate([&](const std::string& key, const std::string& data_id) {

        if (!Server::match(pattern, key)) {
            return;
        }


        auto d = dataMap.find(data_id);

        if (d != dataMap.end()) {


            std::shared_ptr<RikerIO::Data> data = d->second;

            bool isPrivate = dataMap.isPrivate(d->first);

            DataListResponseItem data_item = {
                d->first,
                data->get_offset(),
                data->get_type(),
                dataMap.getSemaphore(d->first),
                isPrivate
            };

            LinkListItem linkItem =  { key, true, data_item };
            response.list.push_back(linkItem);

        } else {

            LinkListItem linkItem = { key, false, { data_id, MemoryPosition(), Type(), -1, false } };

            linkItem.has_data = false;
            linkItem.data_item.id = data_id;

            response.list.push_back(linkItem);

        }


    });


}


Json::Value Server::link_get(const std::string& id) {

    (void)(id);

    Json::Value result;
    result["code"] = 0;

    return result;


}

bool Server::match(const std::string& pattern, const std::string& target) {

    int res = fnmatch(pattern.c_str(), target.c_str(), 0);

    return res == 0;

}

void Server::makeLinksPersistent() {

    while (persistentThreadRunning) {

        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (persistentChangeCount == 0) {
            continue;
        }

        {

            const std::lock_guard<std::mutex> lock(persistent_mutex);

            linkMap.serialize();

            persistentChangeCount = 0;

        }

    }

    {

        const std::lock_guard<std::mutex> lock(persistent_mutex);

        linkMap.serialize();

    }

}
