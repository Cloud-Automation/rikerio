#include "gtest/gtest.h"
#include "client/response.h"
#include "client/response/config-get.h"
#include "client/response/memory-alloc.h"
#include "client/response/memory-list.h"
#include "client/response/data-remove.h"
#include "client/response/data-list.h"
#include "client/response/link-add.h"
#include "client/response/link-remove.h"
#include "client/response/link-list.h"

void JsonParse(const std::string& str, Json::Value& root) {

    JSONCPP_STRING err;
    Json::CharReaderBuilder builder;

    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    reader->parse(str.c_str(), str.c_str() + str.length(), &root, &err);

}

TEST(Response, CreateWithErrorJson) {

    std::string responseStr = "{\"code\": 1, \"message\": \"Some error message.\"}";
    Json::Value responseJson;

    JsonParse(responseStr, responseJson);

    RikerIO::RPCResponse response(responseJson);

    ASSERT_EQ(response.get_code(), 1);
    ASSERT_STREQ(response.get_message().c_str(), "Some error message.");

}

TEST(ConfigGet, CreateWithValidResponse) {

    std::string responseStr = "{\"code\": 0, \"data\": { \"id\": \"default\", \"version\": \"3.0.0\", \"shm\":\"/var/lib/rikerio/default/shm\",\"size\":4096,\"cycle\":10000 }}";
    Json::Value responseJson;

    JsonParse(responseStr, responseJson);

    RikerIO::Response::v1::ConfigGet response(responseJson);

    ASSERT_EQ(response.get_code(), RikerIO::Error::NO_ERROR);
    ASSERT_STREQ(response.get_profile().c_str(), "default");
    ASSERT_STREQ(response.get_version().c_str(), "3.0.0");
    ASSERT_STREQ(response.get_shm_file().c_str(), "/var/lib/rikerio/default/shm");
    ASSERT_EQ(response.get_size(), 4096);
    ASSERT_EQ(response.get_cycle(), 10000);

}

TEST(MemoryAlloc, CreateWithValidResponse) {

    std::string responseStr = "{\"code\":0,\"data\":{\"offset\": 1024,\"token\": \"abc123\"}}";
    Json::Value responseJson;

    JsonParse(responseStr, responseJson);

    RikerIO::Response::v1::MemoryAlloc response(responseJson);

    ASSERT_EQ(response.get_offset(), 1024);
    ASSERT_STREQ(response.get_token().c_str(), "abc123");

}

TEST(MemoryList, CreateWithValidResponse) {

    std::string responseStr = "{\"code\":0,\"data\":[{\"offset\":1024,\"size\":23,\"semaphore\":123},{\"offset\":1025,\"size\":24,\"semaphore\":124},{\"offset\":1026,\"size\":25,\"semaphore\":125}]";
    Json::Value responseJson;

    JsonParse(responseStr, responseJson);

    RikerIO::Response::v1::MemoryList response(responseJson);

    ASSERT_EQ(response.get_items().size(), 3);

    ASSERT_EQ(response.get_items()[0]->get_offset(), 1024);
    ASSERT_EQ(response.get_items()[0]->get_size(), 23);
    ASSERT_EQ(response.get_items()[0]->get_semaphore(), 123);

    ASSERT_EQ(response.get_items()[1]->get_offset(), 1025);
    ASSERT_EQ(response.get_items()[1]->get_size(), 24);
    ASSERT_EQ(response.get_items()[1]->get_semaphore(), 124);

    ASSERT_EQ(response.get_items()[2]->get_offset(), 1026);
    ASSERT_EQ(response.get_items()[2]->get_size(), 25);
    ASSERT_EQ(response.get_items()[2]->get_semaphore(), 125);

}

TEST(DataRemove, CreateWithValidResponse) {

    std::string responseStr = "{\"code\":0,\"data\":{\"count\":3}}";
    Json::Value responseJson;

    JsonParse(responseStr, responseJson);

    RikerIO::Response::v1::DataRemove response(responseJson);

    ASSERT_EQ(response.get_count(), 3);

}

TEST(DataList, CreateWithValidResponse) {

    std::string responseStr = "{\"code\":0,\"data\":[{\"id\":\"data-id-a\",\"type\":\"int8\",\"offset\":1,\"index\":2,\"size\":3,\"semaphore\":4,\"private\":true},{\"id\":\"data-id-b\",\"type\":\"float\",\"offset\":5,\"index\":6,\"size\":7,\"semaphore\":8,\"private\":false},{\"id\":\"data-id-c\",\"type\":\"undefined\",\"offset\":9,\"index\":10,\"size\":11,\"semaphore\":12,\"private\":false}]}";
    Json::Value responseJson;

    JsonParse(responseStr, responseJson);

    RikerIO::Response::v1::DataList response(responseJson);

    ASSERT_EQ(response.get_items().size(), 3);

    ASSERT_STREQ(response.get_items()[0]->get_id().c_str(), "data-id-a");
    ASSERT_EQ(response.get_items()[0]->get_datatype(), RikerIO::Utils::Datatype::INT8);
    ASSERT_EQ(response.get_items()[0]->get_offset(), 1);
    ASSERT_EQ(response.get_items()[0]->get_index(), 2);
    ASSERT_EQ(response.get_items()[0]->get_size(), 3);
    ASSERT_EQ(response.get_items()[0]->get_semaphore(), 4);
    ASSERT_TRUE(response.get_items()[0]->is_private());

    ASSERT_STREQ(response.get_items()[1]->get_id().c_str(), "data-id-b");
    ASSERT_EQ(response.get_items()[1]->get_datatype(), RikerIO::Utils::Datatype::FLOAT);
    ASSERT_EQ(response.get_items()[1]->get_offset(), 5);
    ASSERT_EQ(response.get_items()[1]->get_index(), 6);
    ASSERT_EQ(response.get_items()[1]->get_size(), 7);
    ASSERT_EQ(response.get_items()[1]->get_semaphore(), 8);
    ASSERT_FALSE(response.get_items()[1]->is_private());

    ASSERT_STREQ(response.get_items()[2]->get_id().c_str(), "data-id-c");
    ASSERT_EQ(response.get_items()[2]->get_datatype(), RikerIO::Utils::Datatype::UNDEFINED);
    ASSERT_EQ(response.get_items()[2]->get_offset(), 9);
    ASSERT_EQ(response.get_items()[2]->get_index(), 10);
    ASSERT_EQ(response.get_items()[2]->get_size(), 11);
    ASSERT_EQ(response.get_items()[2]->get_semaphore(), 12);
    ASSERT_FALSE(response.get_items()[2]->is_private());

}

TEST(LinkAdd, CreateWithValidResponse) {

    std::string responseStr = "{\"code\":0,\"data\":{\"count\":7}}";
    Json::Value responseJson;

    JsonParse(responseStr, responseJson);

    RikerIO::Response::v1::LinkAdd response(responseJson);

    ASSERT_EQ(response.get_count(), 7);

}

TEST(LinkRemove, CreateWithValidResponse) {

    std::string responseStr = "{\"code\":0,\"data\":{\"count\":7}}";
    Json::Value responseJson;

    JsonParse(responseStr, responseJson);

    RikerIO::Response::v1::LinkRemove response(responseJson);

    ASSERT_EQ(response.get_count(), 7);

}

TEST(LinkList, CreateWithValidResponse) {

    std::string responseStr = "{\"code\":0,\"data\":[{\"key\":\"link-key-a\",\"id\":\"data-id-a\"},{\"key\":\"link-key-b\",\"id\":\"data-id-b\",\"data\":{\"id\":\"data-id-c\",\"type\":\"int8\",\"offset\":1,\"index\":2,\"size\":3,\"semaphore\":4,\"private\":true}},{\"key\":\"link-key-c\",\"id\":\"data-id-d\",\"data\":{\"id\":\"data-id-e\",\"type\":\"float\",\"offset\":5,\"index\":6,\"size\":7,\"semaphore\":8,\"private\":false}}]}";
    Json::Value responseJson;

    JsonParse(responseStr, responseJson);

    RikerIO::Response::v1::LinkList response(responseJson);

    ASSERT_EQ(response.get_items().size(), 3);

    ASSERT_STREQ(response.get_items()[0]->get_key().c_str(), "link-key-a");
    ASSERT_STREQ(response.get_items()[0]->get_id().c_str(), "data-id-a");
    ASSERT_EQ(response.get_items()[0]->get_data(), nullptr);

    ASSERT_STREQ(response.get_items()[1]->get_key().c_str(), "link-key-b");
    ASSERT_STREQ(response.get_items()[1]->get_id().c_str(), "data-id-b");
    ASSERT_NE(response.get_items()[1]->get_data(), nullptr);
    ASSERT_STREQ(response.get_items()[1]->get_data()->get_id().c_str(), "data-id-c");
    ASSERT_EQ(response.get_items()[1]->get_data()->get_datatype(), RikerIO::Utils::Datatype::INT8);
    ASSERT_EQ(response.get_items()[1]->get_data()->get_offset(), 1);
    ASSERT_EQ(response.get_items()[1]->get_data()->get_index(), 2);
    ASSERT_EQ(response.get_items()[1]->get_data()->get_size(), 3);
    ASSERT_EQ(response.get_items()[1]->get_data()->get_semaphore(), 4);
    ASSERT_TRUE(response.get_items()[1]->get_data()->is_private());

    ASSERT_STREQ(response.get_items()[2]->get_key().c_str(), "link-key-c");
    ASSERT_STREQ(response.get_items()[2]->get_id().c_str(), "data-id-d");
    ASSERT_NE(response.get_items()[2]->get_data(), nullptr);
    ASSERT_STREQ(response.get_items()[2]->get_data()->get_id().c_str(), "data-id-e");
    ASSERT_EQ(response.get_items()[2]->get_data()->get_datatype(), RikerIO::Utils::Datatype::FLOAT);
    ASSERT_EQ(response.get_items()[2]->get_data()->get_offset(), 5);
    ASSERT_EQ(response.get_items()[2]->get_data()->get_index(), 6);
    ASSERT_EQ(response.get_items()[2]->get_data()->get_size(), 7);
    ASSERT_EQ(response.get_items()[2]->get_data()->get_semaphore(), 8);
    ASSERT_FALSE(response.get_items()[2]->get_data()->is_private());


}
