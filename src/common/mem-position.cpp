#include "common/mem-position.h"

std::regex RikerIO::MemoryPosition::RegX("^([1-9]+[0-9]*|0)\\.([0-7])$");

RikerIO::MemoryPosition::MemoryPosition() : byte_offset(0), bit_index(0) { }

RikerIO::MemoryPosition::MemoryPosition(const std::string& str) {

    long t_byte_offset = 0;
    long t_bit_index = 0;

    try {

        std::smatch matches;
        if (std::regex_search(str, matches, RegX)) {
            t_byte_offset = stoi(matches[1].str());
            t_bit_index = stoi(matches[2].str());
        } else {
            t_byte_offset = std::stoi(str);
        }

        if (t_byte_offset < 0 || t_byte_offset > std::numeric_limits<unsigned int>::max()) {
            throw MemoryPositionError("Error converting string to memory position.");
        }

    } catch (std::invalid_argument& e) {
        throw MemoryPositionError("Error converting string to memory position.");
    }

    byte_offset = t_byte_offset;
    bit_index = t_bit_index;

}

RikerIO::MemoryPosition::MemoryPosition(unsigned int byte_offset, unsigned int bit_index) :
    byte_offset(byte_offset), bit_index(bit_index) {

    if (bit_index > 7) {
        throw MemoryPositionError("Bit Index musst be between 0 and 7.");
    }

}

unsigned int RikerIO::MemoryPosition::get_byte_offset() const {
    return byte_offset;
}

unsigned int RikerIO::MemoryPosition::get_bit_index() const {
    return bit_index;
}

RikerIO::MemoryPosition& RikerIO::MemoryPosition::add_byte_offset(unsigned int value) {
    byte_offset += value;
    return *this;
}

RikerIO::MemoryPosition& RikerIO::MemoryPosition::add_bit_offset(unsigned int value) {
    bit_index += value % 8;
    byte_offset += (value - (value % 8)) / 8;
    return *this;
}

std::string RikerIO::MemoryPosition::to_string() const  {
    return std::to_string(byte_offset) + "." + std::to_string(bit_index);
}
