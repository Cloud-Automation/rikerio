#ifndef __RIKERIO_DATA_H__
#define __RIKERIO_DATA_H__

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <exception>

#include "common/type.h"
#include "common/mem-position.h"
#include "server/memory-area.h"


namespace RikerIO  {

class Data {
  public:

    static bool isValidId(const std::string&);

    Data(Type type, MemoryPosition offset) :
        type(type), offset(offset) { }

    bool operator==(Data& a) const {
        return type == a.get_type() && offset == a.get_offset();
    }

    const Type& get_type() const {
        return type;
    }

    MemoryPosition& get_offset() {
        return offset;
    }

    unsigned int getEnd() const {
        return offset.get_byte_offset() + type.get_byte_size();
    }

    bool inRange(unsigned int offs, unsigned int sz) const {

        unsigned int e = offs + sz;

        return (offset.get_byte_offset() >= offs && getEnd() <= e);

    }

  private:

    Type type;
    MemoryPosition offset;

};

using DataPtr = std::shared_ptr<Data>;

}


#endif
