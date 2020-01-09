#include "common/type.h"


std::regex RikerIO::Type::WordSizeX("^([1-9]\\d*)(bit|byte|kilobyte)$");

std::map<RikerIO::Type::Types, unsigned int> RikerIO::Type::TypeSizeMap{
    { UNDEFINED, 0 },
    { BIT, 1 },
    { UINT8, 8 },
    { INT8, 8 },
    { UINT16, 16 },
    { INT16, 16 },
    { INT32, 32 },
    { UINT32, 32 },
    { INT64, 64 },
    { UINT64, 64 },
    { FLOAT, 32 },
    { DOUBLE, 64 }
};

std::map<std::string, RikerIO::Type::Types> RikerIO::Type::StringTypeMap{
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

std::map<RikerIO::Type::Types, std::string> RikerIO::Type::TypeStringMap{
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

RikerIO::Type::Type() : size(0), type(UNDEFINED) { }

RikerIO::Type::Type(const std::string& str) {

    if (StringTypeMap.find(str) != StringTypeMap.end()) {
        type = StringTypeMap[str];
        size = TypeSizeMap[type];
        return;
    }

    type = UNDEFINED;

    std::smatch matches;
    if (!std::regex_search(str, matches, WordSizeX)) {
        throw TypeError("Unknown string for type conversion.");
    }

    unsigned int number = std::stoi(matches[1].str());
    std::string type = matches[2].str();

    if (type == "bit") {
        size = number;
    } else if (type == "byte") {
        size = number * 8;
    } else if (type == "kilobyte") {
        size = number * 8 * 1024;
    }

}

RikerIO::Type::Type(unsigned int bitSize) : size(bitSize), type(UNDEFINED) {

    if (bitSize == 0) {
        throw TypeError("Type size cannot be zero.");
    }

}

RikerIO::Type::Type(Types t) : type(t) {

    if (t == UNDEFINED) {
        throw TypeError("Cannot specify type bit size of type undefined.");
    }

    size = TypeSizeMap[t];

}

unsigned int RikerIO::Type::get_bit_size() const {
    return size;
}

unsigned int RikerIO::Type::get_byte_size() const {
    unsigned int rest = size % 8;
    unsigned int res = (size - rest) / 8;
    return rest > 0 ? res + 1 : res;
}

RikerIO::Type::Types RikerIO::Type::get_type() const {
    return type;
}

const std::string RikerIO::Type::to_string() const {

    if (type != UNDEFINED) {
        return TypeStringMap[type];
    }

    if (size % 8 != 0) {
        return std::to_string(size) + "bit";
    }

    if (size >= 8 && size < (8 * 1024)) {
        return std::to_string(size / 8) + "byte";
    }

    return std::to_string(size / (8*1024)) + "kilobyte";

}
