
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
    const std::string& id,
    unsigned int size,
    unsigned int cycle) :
    id(id),
    size(size),
    cycle(cycle),
    sh_mem(size, RikerIO::Config::CreateShmPath(id)),
    memory(size),
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

void Server::config_get(RikerIO::ConfigResponse& response) {

    response.profile = id;
    response.version = std::string(RIO_VERSION_STRING);
    response.shmFile = RikerIO::Config::CreateShmPath(id);
    response.size = size;
    response.defaultCycle = cycle;

}

void Server::memory_alloc(MemoryAllocRequest& request, MemoryAllocResponse& response) {

    const std::lock_guard<std::mutex> lock(Server::alloc_mutex);

    if (request.size <= 0) {
        throw ServerError(BAD_REQUEST, "Size musst be > 0.");
    }

    try  {

        std::shared_ptr<MemoryArea> ma = memory.alloc(request.size);

        response.offset = ma->get_offset();
        response.token = ma->get_token();
        response.semaphore = ma->get_semaphore();

    } catch (Token::TokenException& e) {
        throw ServerError(GENTOKEN_ERROR, "Internal Error (1).");
    } catch (OutOfSpaceError& e) {
        throw ServerError(OUTOFSPACE_ERROR, "Not enough memory for allocation.");
    }

}

void Server::memory_dealloc(MemoryDeallocRequest& request) {

    /* get offset by token */

    if (memory.get_area_from_token(request.token) == nullptr) {
        throw ServerError(UNAUTHORIZED_ERROR, "Token not found.");
    }

    std::shared_ptr<MemoryArea> maFromToken = memory.get_area_from_token(request.token);
    std::shared_ptr<MemoryArea> maFromDealloc = memory.dealloc(maFromToken->get_offset());

    if (maFromDealloc == nullptr) {
        throw ServerError(INTERNAL_ERROR, "Internal Error (1).");
    }

    dataMap.remove_by_range(maFromToken->get_offset(), maFromToken->get_size());

}

void Server::memory_list(MemoryListResponse& response) {

    for (auto ma : memory) {
        response.data.push_back(ma);
    }

}

void Server::memory_get(MemoryGetRequest& request, MemoryGetResponse& response) {

    for (auto ma : memory) {

        if (ma->get_offset() != static_cast<unsigned int>(request.offset)) {
            continue;
        }

        response.data = ma;

        return;

    }

    throw ServerError(NOTFOUND_ERROR, "Memory Area not found.");

}

void Server::data_add(
    DataAddRequest& req,
    DataAddResponse& res) {

    spdlog::debug("data_add({},{},{},{})",req.id, req.token, req.type.to_string(), req.offset.to_string());

    std::shared_ptr<MemoryArea> memArea = nullptr;
    std::shared_ptr<Data> entry = nullptr;

    bool hasToken = req.token.length() > 0;

    if (!Data::isValidId(req.id)) {
        throw ServerError(BAD_REQUEST, "Data ID is not valid.");
    }

    /* get offset by token */

    if (hasToken) {

        memArea = memory.get_area_from_token(req.token);

        if (!memArea) {
            throw ServerError(UNAUTHORIZED_ERROR, "Token not found.");
        }

        spdlog::debug("found a valid token.");

    } else {

        memArea = memory.get_area_from_range(req.offset.get_byte_offset(), req.type.get_byte_size());

    }

    if (!memArea) {
        throw ServerError(BAD_REQUEST, "Data not inside memory area.");
    }

    spdlog::debug("found a valid memory area.");

    if (hasToken) {
        spdlog::debug("adding offset.");
        req.offset.add_byte_offset(memArea->get_offset());
    }

    std::shared_ptr<RikerIO::Data> d = std::make_shared<RikerIO::Data>(req.type, req.offset);

    bool value = false;
    if (hasToken) {
        value = dataMap.add(req.id, req.token, d);
    } else {
        value = dataMap.add(req.id, d);
    }

    spdlog::debug("Adding data result {}.", value);

    res.type = req.type;
    res.offset = req.offset;
    res.semaphore = memArea->get_semaphore();

}

void Server::data_remove(DataRemoveRequest& request, DataRemoveResponse& response) {

    for (auto d : dataMap) {

        if (!Server::match(request.pattern, d.first)) {
            continue;
        }

        bool removeSuccessfull = false;

        if (request.token.length() == 0) {
            removeSuccessfull = dataMap.remove(d.first);
        } else {
            removeSuccessfull = dataMap.remove(d.first, request.token);
        }

        response.count += removeSuccessfull ? 1 : 0;

    }

}


void Server::data_list(DataListRequest& request, DataListResponse& response) {

    for (auto d : dataMap) {

        if (!Server::match(request.pattern, d.first)) {
            continue;
        }


        std::shared_ptr<RikerIO::Data> data = d.second;

        bool is_private = dataMap.is_private(d.first);

        DataListResponseItem item;
        item.id = d.first,
        item.offset = data->get_offset(),
        item.type = data->get_type(),
        item.semaphore = dataMap.get_semaphore(d.first),
        item.is_private = is_private;

        response.list.push_back(item);

    }

}


void Server::link_add(LinkAddRequest& request, LinkAddResponse& response) {

    const std::lock_guard<std::mutex> lock(persistent_mutex);

    for (std::string s : request.ids) {
        if (linkMap.add(request.key, s)) {
            response.counter += 1;
        }
    }

    /* start persistent thread */

    persistentChangeCount += response.counter;

}

void Server::link_remove(LinkRemoveRequest& request, LinkRemoveResponse& response) {

    const std::lock_guard<std::mutex> lock(persistent_mutex);

    std::set<std::string> keys = linkMap.keys();

    for (auto key : keys) {

        if (!Server::match(request.pattern, key)) {

            continue;

        }

        if (request.ids.size() == 0) {

            response.counter += linkMap.remove(key);

        } else {

            for (auto data : request.ids) {
                response.counter += linkMap.remove(key, data) ? 1 : 0;
            }

        }
    }

    persistentChangeCount += response.counter;

}


void Server::link_list(LinkListRequest& request, LinkListResponse& response) {

    const std::lock_guard<std::mutex> lock(persistent_mutex);

    linkMap.iterate([&](const std::string& key, const std::string& data_id) {

        if (!Server::match(request.pattern, key)) {
            return;
        }


        auto d = dataMap.find(data_id);

        if (d != dataMap.end()) {


            std::shared_ptr<RikerIO::Data> data = d->second;

            bool is_private = dataMap.is_private(d->first);

            DataListResponseItem item;
            item.id = d->first;
            item.offset = data->get_offset();
            item.type = data->get_type();
            item.semaphore = dataMap.get_semaphore(d->first);
            item.is_private = is_private;

            LinkListItem linkItem(key, d->first, &item);
            response.list.push_back(linkItem);

        } else {

            LinkListItem linkItem(key, d->first);

            linkItem.has_data = false;
            linkItem.data_item.id = data_id;

            response.list.push_back(linkItem);

        }


    });


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
