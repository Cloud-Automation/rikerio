#include "common/utils.h"

const std::vector<unsigned int> RikerIO::Utils::DatatypeSize = { 0, 1, 8, 8, 16, 16, 32, 32, 64, 64, 32, 64 };
std::regex RikerIO::Utils::wordSizeX("^([1-9]\\d*)(bit|byte|kilobyte)$");

std::string RikerIO::Utils::GetStringFromType(const Datatype& type) {

    static std::map<Datatype, std::string> typeStringMap{
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

    if (typeStringMap.find(type) == typeStringMap.end()) {
        return "undefined";
    }

    return typeStringMap[type];
}

RikerIO::Utils::Datatype RikerIO::Utils::GetTypeFromString(const std::string& str) {

    static std::map<std::string, Datatype> stringTypeMap{
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

bool RikerIO::Utils::IsWordSize (const std::string& str) {
    return std::regex_match(str, wordSizeX);
}

unsigned int RikerIO::Utils::GetBitSize (const std::string& str) {

    std::smatch matches;
    if (!std::regex_search(str, matches, wordSizeX)) {
        return 0;
    }

    unsigned int number = std::stoi(matches[1].str());
    std::string type = matches[2].str();

    if (type == "bit") {
        return number;
    } else if (type == "byte") {
        return number * 8;
    } else if (type == "kilobyte") {
        return number * 8 * 1024;
    }

    return 0;

}
