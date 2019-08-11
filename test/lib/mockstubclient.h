#ifndef __MOCK_STUB_CLIENT_H__
#define __MOCK_STUB_CLIENT_H__

#include "gmock/gmock.h"
#include "client/abstract-client.h"

class MockClient : public RikerIO::AbstractClient {

  public:

    MOCK_METHOD(std::string, task_register, (const std::string& name, int pid, bool track), (override));
    MOCK_METHOD(void, task_unregister, (const std::string& token), (override));
    MOCK_METHOD(std::set<std::string>, task_list, (), (override));
    MOCK_METHOD(unsigned int, memory_alloc, (int, const std::string&), (override));
    MOCK_METHOD(void, memory_dealloc, (int, const std::string&), (override));
    MOCK_METHOD(void, data_create, (const RikerIO::Data&, const std::string&, const std::string&), (override));
    /*    MOCK_METHOD(Json::Value, data_remove, (const std::string&, const std::string&), (override));
        MOCK_METHOD(Json::Value, data_list, (), (override));
        MOCK_METHOD(Json::Value, data_get, (const std::string&, const std::string&), (override));
        MOCK_METHOD(Json::Value, link_add, (const std::string&, const std::string&), (override));
        MOCK_METHOD(Json::Value, link_remove, (const std::string&, const std::string&), (override));
        MOCK_METHOD(Json::Value, link_list, (const std::string&), (override));
        MOCK_METHOD(Json::Value, link_get, (const std::string&), (override));
        MOCK_METHOD(Json::Value, link_updates, (const std::string&), (override)); */
};


#endif
