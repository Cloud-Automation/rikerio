#ifndef __RIKERIO_CLIENT_ALLOCATION_H__
#define __RIKERIO_CLIENT_ALLOCATION_H__

#include "client/abstract-client.h"
#include "client/data.h"
#include "common/error.h"

namespace RikerIO {
namespace Client {
class Task;

class Allocation {
  public:
    Allocation(unsigned int offset, unsigned int size);

    unsigned int getOffset();
    unsigned int getSize();

    template<typename T>
    Client::Data<T>& createData(unsigned int byteOffset, unsigned int bitOffset) {

        if ((byteOffset + sizeof(T)) >= size) {
            throw OutOfScopeError();
        }

        std::shared_ptr<Client::Data<T>> data = std::make_shared<Client::Data<T>>(offset + byteOffset, bitOffset);

        dataSet.insert(data);

        return *data;

    }

    template<typename T>
    Client::Data<T>& createData() {

        unsigned int oldOffset = runningOffset;

        /* auto incremente running offset */
        if (std::is_same<T, bool>::value) {

            runningOffset += 1;

        } else {

            runningOffset += runningOffset % 8;
            runningOffset += sizeof(T) * 8;

        }

        unsigned int biOf = oldOffset % 8;
        unsigned int byOf = (oldOffset - biOf) / 8;

        return createData<T>(byOf, biOf);

    }


  private:
    unsigned int offset;
    unsigned int size;

    unsigned int runningOffset;

    std::set<std::shared_ptr<RikerIO::Data>> dataSet;

};

}

}

#endif
