#include "gtest/gtest.h"
#include "helper.h"
#include "client/request.h"
#include "client/request/memory-alloc.h"
#include "client/request/memory-dealloc.h"
#include "client/request/data-add.h"
#include "client/request/data-remove.h"
#include "client/request/data-list.h"
#include "client/request/link-add.h"
#include "client/request/link-remove.h"
#include "client/request/link-list.h"

TEST(Request, MemoryAlloc) {

    std::string exRequestStr = "{\"size\":1024}";
    Json::Value exRequestJson;

    JsonParse(exRequestStr, exRequestJson);

    RikerIO::Request::v1::MemoryAlloc request(1024);

    AssertJsonEqual(exRequestJson, request.create_params());

}

TEST(Request, MemoryDealloc) {

    std::string exRequestStr = "{\"token\":\"abc123\"}";
    Json::Value exRequestJson;

    JsonParse(exRequestStr, exRequestJson);

    RikerIO::Request::v1::MemoryDealloc request("abc123");

    AssertJsonEqual(exRequestJson, request.create_params());

}

TEST(Request, DataAdd) {

    std::string exRequestStr = "{\"id\":\"data-id-a\",\"data\":{\"token\":\"token-a\",\"type\":\"int8\",\"offset\":10,\"index\":1,\"size\":8}}";
    Json::Value exRequestJson;

    JsonParse(exRequestStr, exRequestJson);

    RikerIO::Request::v1::DataAdd request(
        "data-id-a",
        "token-a",
        RikerIO::Utils::Datatype::INT8,
        8,
        10,
        1);

    AssertJsonEqual(exRequestJson, request.create_params());

}

TEST(Request, DataRemoveWithoutToken) {

    std::string exRequestStr = "{\"pattern\":\"pattern-a\"}";
    Json::Value exRequestJson;

    JsonParse(exRequestStr, exRequestJson);

    RikerIO::Request::v1::DataRemove request("pattern-a");

    AssertJsonEqual(exRequestJson, request.create_params());

}

TEST(Request, DataRemoveWithToken) {

    std::string exRequestStr = "{\"pattern\":\"pattern-a\", \"token\":\"token-a\"}";
    Json::Value exRequestJson;

    JsonParse(exRequestStr, exRequestJson);

    RikerIO::Request::v1::DataRemove request("pattern-a", "token-a");

    AssertJsonEqual(exRequestJson, request.create_params());

}

TEST(Request, DataList) {

    std::string exRequestStr = "{\"pattern\":\"*\"}";
    Json::Value exRequestJson;

    JsonParse(exRequestStr, exRequestJson);

    RikerIO::Request::v1::DataList request("*");

    AssertJsonEqual(exRequestJson, request.create_params());

}

TEST(Request, LinkAdd) {

    std::string exRequestStr = "{\"key\":\"link-key-a\",\"data\":[\"data-id-a\",\"data-id-b\",\"data-id-c\"]}";
    Json::Value exRequestJson;

    JsonParse(exRequestStr, exRequestJson);

    RikerIO::Request::v1::LinkAdd request("link-key-a", { "data-id-a", "data-id-b", "data-id-c" });

    AssertJsonEqual(exRequestJson, request.create_params());

}

TEST(Request, LinkRemove) {

    std::string exRequestStr = "{\"pattern\":\"pattern-a\",\"data\":[\"data-id-a\",\"data-id-b\",\"data-id-c\"]}";
    Json::Value exRequestJson;

    JsonParse(exRequestStr, exRequestJson);

    RikerIO::Request::v1::LinkRemove request("pattern-a", { "data-id-a", "data-id-b", "data-id-c" });

    AssertJsonEqual(exRequestJson, request.create_params());

}

TEST(Request, LinkList) {

    std::string exRequestStr = "{\"pattern\":\"pattern-a\"}";
    Json::Value exRequestJson;

    JsonParse(exRequestStr, exRequestJson);

    RikerIO::Request::v1::LinkList request("pattern-a");

    AssertJsonEqual(exRequestJson, request.create_params());


}
