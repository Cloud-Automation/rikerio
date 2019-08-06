#include "common/data.h"
#include <fnmatch.h>

using namespace RikerIO;


Data::Type Data::GetTypeFromString(const std::string& str) {

    static std::map<std::string, Data::Type> stringTypeMap{
        { "undefined", UNDEFINED },
        { "bit", BIT },
        { "uint8", UINT8},
        { "int8", INT8 },
        { "uint16", UINT16 },
        { "int16", INT16 },
        { "uint32", UINT32 },
        { "int32", INT32 },
        { "uint64", UINT64 },
        { "int64", INT64 },
        { "float", FLOAT },
        { "double", DOUBLE }
    };


    if (stringTypeMap.find(str) == stringTypeMap.end()) {
        return UNDEFINED;
    }

    return stringTypeMap[str];

}

std::string Data::GetStringFromType(Data::Type type) {

    static std::map<Data::Type, std::string> typeStringMap{
        { UNDEFINED, "undefined" },
        { BIT, "bit" },
        { UINT8, "uint8" },
        { INT8, "int8" },
        { UINT16, "uint16" },
        { INT16, "int16" },
        { UINT32, "uint32" },
        { INT32, "int32" },
        { UINT64, "uint64" },
        { INT64, "int64" },
        { FLOAT, "float" },
        { DOUBLE, "double" }
    };

    return typeStringMap[type];

}

DataFactory::DataFactory() :
    idDataMap() {


}

bool DataFactory::create(std::string id, Data data) {

    if (idDataMap.find(id) != idDataMap.end()) {
        /* id already used */
        return false;
    }

    idDataMap[id] = std::make_shared<Data>(data);

    return true;

}

bool DataFactory::remove(std::string id) {

    auto it = idDataMap.find(id);

    if (it == idDataMap.end()) {
        /* no data with such id */
        return false;
    }

    idDataMap.erase(it);

    return true;

}


std::vector<std::string> DataFactory::filter(std::string pattern) {

    std::vector<std::string> result;

    for (auto a : idDataMap) {

        std::string id = a.first;

        if (pattern.length() > 0) {
            result.push_back(id);
            continue;
        }

        int matchRes = fnmatch(pattern.c_str(), id.c_str(), FNM_EXTMATCH);

        if (matchRes == FNM_NOMATCH) {
            continue;
        }

        result.push_back(id);

    }

    return result;

}

void DataFactory::clear() {

    idDataMap.clear();

}
