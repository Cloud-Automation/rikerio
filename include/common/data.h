#ifndef __RIKERIO_DATA_H__
#define __RIKERIO_DATA_H__

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <exception>

#include "common/datatypes.h"
#include "server/memory-area.h"


namespace RikerIO  {

class InvalidDataError : public std::exception {

};

class Data {
  public:

    class OutOfScopeError : public std::exception {};

    static bool isValidId(const std::string&);

    static Datatype GetTypeFromString(const std::string& str);
    static std::string GetStringFromType(Datatype type);

    Data(
        MemoryAreaPtr memArea,
        const std::string token,
        const std::string& type,
        unsigned int offset) :
        Data(memArea, token, GetTypeFromString(type), offset) { }

    Data(MemoryAreaPtr memArea, const std::string token, RikerIO::Datatype type, unsigned int offset) :
        Data(memArea, token, type, offset, RikerIO::DatatypeSize[type]) { }

    Data(MemoryAreaPtr memArea, const std::string token, RikerIO::Datatype type, unsigned int offset, unsigned int size) :
        Data(memArea, token, type, offset, 0, size) { }

    Data(MemoryAreaPtr memArea, const std::string token, RikerIO::Datatype type, unsigned int offset, unsigned int index, unsigned int size) :
        memArea(memArea), token(token), type(type), offset(offset), index(index), size(size) {

        if (index > 0 && size > (index - 8)) {
            throw InvalidDataError();
        }

    }

    bool operator==(const Data& a) const {
        return type == a.getType() &&
               offset == a.getOffset() &&
               index == a.getIndex() &&
               size == a.getSize();
    }

    MemoryAreaPtr getMemoryArea() const {
        return memArea;
    }

    const std::string& getToken() const {
        return token;
    }

    RikerIO::Datatype getType() const {
        return type;
    }
    unsigned int getOffset() const {
        return offset;
    }
    unsigned int getIndex() const {
        return index;
    }
    unsigned int getSize() const {
        return size;
    }
    unsigned int getByteSize() const {
        unsigned int rest = size % 8;
        unsigned int res = (size - rest) / 8;
        return rest > 0 ? res + 1 : res;
    }
    unsigned int getEnd() const {
        return offset + getByteSize();
    }

  private:
    MemoryAreaPtr memArea;
    const std::string token;

    RikerIO::Datatype type;
    const unsigned int offset;
    const unsigned int index;
    const unsigned int size;


};

class DataFactoryEntry {
  public:
    DataFactoryEntry(std::string label, std::string token, std::shared_ptr<Data> data, std::shared_ptr<MemoryArea> memArea) : label(label), token(token), data(data), memArea(memArea) {
    }

    const std::string& getLabel() const {
        return label;
    }

    const std::string& getToken() const {
        return token;
    }

    std::shared_ptr<Data> getData() const {
        return data;
    }

    std::shared_ptr<MemoryArea> getMemoryArea() const {
        return memArea;
    }

  private:
    std::string label;
    std::string token;
    std::shared_ptr<Data> data;
    std::shared_ptr<MemoryArea> memArea;

};

class DataFactory : public std::map<std::string, std::shared_ptr<DataFactoryEntry>> {

};


}


#endif
