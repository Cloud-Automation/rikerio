#ifndef __RIKERIO_MEMORY_POSITION_H__
#define __RIKERIO_MEMORY_POSITION_H__

#include "regex"

namespace RikerIO {

class MemoryPosition {

  public:
    class MemoryPositionError : public std::exception {
      public:
        MemoryPositionError(const std::string& msg) : msg(msg) { };
        const std::string& getMessage() {
            return msg;
        }
      private:
        const std::string msg;
    };

    bool operator==(const MemoryPosition& a) const {
        return byte_offset == a.get_byte_offset() && bit_index == a.get_bit_index();
    }

    bool operator<(const MemoryPosition& a) const {
        return byte_offset <= a.get_byte_offset() && bit_index < a.get_bit_index();
    }

    bool operator>(const MemoryPosition& a) const {
        return byte_offset >= a.get_byte_offset() && bit_index > a.get_bit_index();
    }

    MemoryPosition();
    MemoryPosition(const std::string&);
    MemoryPosition(unsigned int byteOffset, unsigned int bitIndex = 0);

    unsigned int get_byte_offset() const;
    unsigned int get_bit_index() const;

    MemoryPosition& add_byte_offset(unsigned int);
    MemoryPosition& add_bit_offset(unsigned int);

    std::string to_string() const;

  private:

    static std::regex RegX;

    unsigned int byte_offset = 0;
    unsigned int bit_index = 0;

};


}


#endif
