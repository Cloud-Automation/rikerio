#ifndef __RIKERIO_DATA_H__
#define __RIKERIO_DATA_H__

#include <map>
#include <memory>
#include <vector>
#include <string>
#include "common/datatypes.h"

namespace RikerIO  {

class Data {
  public:
    static Datatype GetTypeFromString(const std::string& str);
    static std::string GetStringFromType(Datatype type);

    Data() : type(Datatype::UNDEFINED), byteOffset(0), bitOffset(0), bitSize(0) {
    }

    Data(RikerIO::Datatype type, unsigned int byteOffset, unsigned int bitOffset, unsigned int bitSize) :
        type(type), byteOffset(byteOffset), bitOffset(bitOffset), bitSize(bitSize) {
    }

    bool operator==(const Data& a) const {
        return type == a.getDatatype() &&
               byteOffset == a.getByteOffset() &&
               bitOffset == a.getBitOffset() &&
               bitSize == a.getBitSize();
    }

    RikerIO::Datatype getDatatype() const {
        return type;
    }
    unsigned int getByteOffset() const {
        return byteOffset;
    }
    unsigned int getBitOffset() const {
        return bitOffset;
    }
    unsigned int getBitSize() const {
        return bitSize;
    }

  protected:
    RikerIO::Datatype type;
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
