
#ifndef ABSTRACT_STUB_STUBCLIENT_H_
#define ABSTRACT_STUB_STUBCLIENT_H_

#include <set>
#include <vector>
#include <memory>
#include <string>

namespace RikerIO {

class AbstractClient {
  public:

    class ClientError : public std::exception {
      public:
        ClientError(int code, const std::string msg) : code(code), msg(msg) { }
        int getCode() {
            return code;
        }
        const std::string getMessage() {
            return msg;
        }
      private:
        int code;
        const std::string msg;
    };

    struct ConfigGetResponse {
        std::string profile;
        std::string version;
        std::string shmFile;
        unsigned int size;
        unsigned int defaultCycle; // in us
    };

    struct MemoryAllocResponse {
        unsigned int offset;
        std::string token;
    };

    struct MemoryListItem {
        unsigned int offset;
        unsigned int size;
        int semaphore;
    };

    struct MemoryListResponse {
        std::vector<MemoryListItem> list;
    };

    struct DataCreateRequest {
        std::string type;
        unsigned int size;
        unsigned int index;
        unsigned int offset;
    };

    struct DataListItem {
        std::string id;
        int semaphore;
        std::string datatype;
        unsigned int offset;
        unsigned int index;
        unsigned int size;
        bool isPrivate;
    };

    struct DataListResponse {
        std::vector<DataListItem> list;
    };

    struct LinkListItem {
        std::string key;
        std::string id;
        std::shared_ptr<DataListItem> data;
    };

    struct LinkListResponse {
        std::vector<LinkListItem> list;
    };

    virtual ~AbstractClient() { };
    virtual void config_get(ConfigGetResponse&) = 0;
    virtual void memory_alloc(unsigned int size, MemoryAllocResponse&) = 0;
    virtual void memory_dealloc(const std::string& token) = 0;
    virtual void memory_list(MemoryListResponse&) = 0;

    virtual void data_create(const std::string& token, const std::string& id, DataCreateRequest) = 0;
    virtual void data_list(const std::string& pattern, DataListResponse&)  = 0;
    virtual void data_remove(const std::string& id, const std::string& token, unsigned int&)  = 0;

    virtual void link_add(const std::string&, std::vector<std::string>&, unsigned int&) = 0;
    virtual void link_list(const std::string& pattern, LinkListResponse&) = 0;

    /*virtual Json::Value data_remove(const std::string& id, const std::string& token)  = 0;
    virtual Json::Value data_get(const std::string& id, const std::string& token) = 0;
    virtual Json::Value link_add(const std::string& dataId, const std::string& linkId) = 0;
    virtual Json::Value link_remove(const std::string& dataId, const std::string& linkId) = 0;
    virtual Json::Value link_get(const std::string& id) = 0;
    virtual Json::Value link_updates(const std::string& token) = 0;
    */
};

}

#endif //ABSTRACT_STUB_STUBCLIENT_H_
