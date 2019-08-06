#ifndef __RIKERIO_DATA_H__
#define __RIKERIO_DATA_H__

#include <map>
#include <memory>
#include <vector>

namespace RikerIO  {

class Data {

  public:

    enum Type {
        UNDEFINED = 0,
        BIT = 1,
        UINT8 = 2,
        INT8 = 3,
        UINT16 = 4,
        INT16 = 5,
        UINT32 = 6,
        INT32 = 7,
        UINT64 = 8,
        INT64 = 9,
        FLOAT = 10,
        DOUBLE = 11
    };

    static Type GetTypeFromString(const std::string&);
    static std::string GetStringFromType(const Type);

    Data(Type type, unsigned int byteOffset, unsigned int bitOffset, unsigned int bitSize) :
        type(type),
        byteOffset(byteOffset),
        bitOffset(bitOffset),
        bitSize(bitSize) {

    }

    Type getType() {
        return type;
    }

    unsigned int getByteOffset() {
        return byteOffset;
    }

    unsigned int getBitOffset() {
        return bitOffset;
    }

    unsigned int getBitSize() {
        return bitSize;
    }

  private:

    Type type;
    unsigned int byteOffset;
    unsigned int bitOffset;
    unsigned int bitSize;

};

class DataFactory {

  public:
    DataFactory();

    bool create(std::string, Data);
    bool remove(std::string);
    std::vector<std::string> filter(std::string);

    std::shared_ptr<Data> operator[](std::string id) {
        return idDataMap[id];
    }

    void clear();

  private:
    std::map<std::string, std::shared_ptr<Data>> idDataMap;

};


}


#endif
