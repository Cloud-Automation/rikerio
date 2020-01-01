#ifndef __RIKERIO_DATA_H__
#define __RIKERIO_DATA_H__

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <exception>

#include "common/utils.h"
#include "server/memory-area.h"


namespace RikerIO  {

class InvalidDataError : public std::exception {

};

class Data {
  public:

    class OutOfScopeError : public std::exception {};

    static bool isValidId(const std::string&);

    Data(
        const std::string& type,
        unsigned int offset) :
        Data(Utils::GetTypeFromString(type), offset) { }

    Data(Utils::Datatype type, unsigned int offset) :
        Data(type, offset, Utils::DatatypeSize[type]) { }

    Data(Utils::Datatype type, unsigned int offset, unsigned int size) :
        Data(type, offset, 0, size) { }

    Data(Utils::Datatype type, unsigned int offset, unsigned int index, unsigned int size) :
        type(type), offset(offset), index(index), size(size) {

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

    Utils::Datatype getType() const {
        return type;
    }
    unsigned int getOffset() const {
        return offset;
    }

    void addOffset(unsigned int value) {
        offset += value;
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

    bool inRange(unsigned int offs, unsigned int sz) const {

        unsigned int e = offs + sz;

        return (offset >= offs && getEnd() <= e);

    }

  private:

    Utils::Datatype type;
    unsigned int offset;
    const unsigned int index;
    const unsigned int size;


};

using DataPtr = std::shared_ptr<Data>;

}


#endif
