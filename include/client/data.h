#ifndef __RIKERIO_CLIENT_DATA_H__
#define __RIKERIO_CLIENT_DATA_H__

#include <type_traits>
#include <stdint.h>
#include "common/data.h"
#include "common/datatypes.h"
#include "common/readable.h"
#include "common/writeable.h"

namespace RikerIO {

namespace Client {

class DataInterface {
  public:
    virtual void init(uint8_t*) = 0;
};

template <typename T>
class Data : public RikerIO::Data, public Readable, public Writeable, public DataInterface {

  public:
    Data(unsigned int byteOffset, unsigned int bitOffset) :
        RikerIO::Data(Datatype::UNDEFINED, byteOffset, bitOffset, 0) {

        setup();

    };

    void init(uint8_t* p) {
        ptr = p;
    }

    void read() {

    }

    void write() {

    }

  private:
    uint8_t* ptr;

    void setup() {

        type = Datatype::UNDEFINED;
        bitSize = sizeof(T) * 8;

        if (std::is_same<T, bool>::value) {

            type = Datatype::BIT;
            bitSize = 1;

        } else if (std::is_same<T, uint8_t>::value) {

            type = Datatype::UINT8;
            bitSize = 8;

        } else if (std::is_same<T, int8_t>::value) {

            type = Datatype::INT8;
            bitSize = 8;

        } else if (std::is_same<T, uint16_t>::value) {

            type = Datatype::UINT16;
            bitSize = 16;

        } else if (std::is_same<T, int16_t>::value) {

            type = Datatype::INT16;
            bitSize = 16;

        } else if (std::is_same<T, uint32_t>::value) {

            type = Datatype::UINT32;
            bitSize = 32;

        } else if (std::is_same<T, int32_t>::value) {

            type = Datatype::INT32;
            bitSize = 32;

        } else if (std::is_same<T, uint64_t>::value) {

            type = Datatype::UINT64;
            bitSize = 64;

        } else if (std::is_same<T, int64_t>::value) {

            type = Datatype::INT64;
            bitSize = 64;

        } else if (std::is_same<T, float>::value) {

            type = Datatype::FLOAT;
            bitSize = 32;

        } else if (std::is_same<T, double>::value) {

            type = Datatype::DOUBLE;
            bitSize = 64;

        }


    }

};

}


}


#endif
