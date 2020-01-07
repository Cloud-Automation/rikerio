#ifndef __RIKERIO_TEST_HELPER_H__
#define __RIKERIO_TEST_HELPER_H__

#include "gtest/gtest.h"
#include "json/json.h"

static void JsonParse(const std::string& str, Json::Value& root) {

    JSONCPP_STRING err;
    Json::CharReaderBuilder builder;

    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    reader->parse(str.c_str(), str.c_str() + str.length(), &root, &err);

}

static void AssertJsonEqual(const Json::Value& a, const Json::Value& b) {

    ASSERT_STREQ(a.toStyledString().c_str(), b.toStyledString().c_str());

}



#endif
