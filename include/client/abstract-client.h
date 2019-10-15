
#ifndef ABSTRACT_STUB_STUBCLIENT_H_
#define ABSTRACT_STUB_STUBCLIENT_H_

#include <set>
#include <string>
#include "common/datatypes.h"
#include "common/data.h"

namespace RikerIO {

class AbstractClient {
  public:

    using TaskList = std::set<std::string>;

    virtual ~AbstractClient() { };
    virtual std::string task_register(const std::string& name, int pid, bool track)  = 0;
    virtual void task_unregister(const std::string& token) = 0;
    virtual void task_list(TaskList&) = 0;
    virtual unsigned int memory_alloc(int size, const std::string& token) = 0;
    virtual void memory_dealloc(int offset, const std::string& token) = 0;
    /*virtual Json::Value memory_inspect() = 0; */
    virtual void data_create(const Data& data, const std::string& id, const std::string& token) = 0;
    /*virtual Json::Value data_remove(const std::string& id, const std::string& token)  = 0;
    virtual Json::Value data_list(const std::string& id)  = 0;
    virtual Json::Value data_get(const std::string& id, const std::string& token) = 0;
    virtual Json::Value link_add(const std::string& dataId, const std::string& linkId) = 0;
    virtual Json::Value link_remove(const std::string& dataId, const std::string& linkId) = 0;
    virtual Json::Value link_list(const std::string& pattern) = 0;
    virtual Json::Value link_get(const std::string& id) = 0;
    virtual Json::Value link_updates(const std::string& token) = 0;
    */
};

}

#endif //ABSTRACT_STUB_STUBCLIENT_H_
