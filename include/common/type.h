#ifndef __RIKERIO_TYPE_H__
#define __RIKERIO_TYPE_H__

#include "string"
#include "map"
#include "regex"

namespace RikerIO {

class Type {

  public:

    class TypeError : public std::exception {
      public:
        TypeError(const std::string& msg) : msg(msg) { };
        const std::string& getMessage() {
            return msg;
        }
      private:
        const std::string msg;
    };

    enum Types {
        UNDEFINED,
        BIT,
        UINT8,
        INT8,
        UINT16,
        INT16,
        UINT32,
        INT32,
        UINT64,
        INT64,
        FLOAT,
        DOUBLE
    };

    Type();
    Type(const std::string&);
    Type(unsigned int bitSize);
    Type(Types);

    bool operator==(const Type& a) const {
        return size == a.get_bit_size() && type == a.get_type();
    }

    unsigned int get_bit_size() const;
    unsigned int get_byte_size() const;
    Types get_type() const;

    const std::string to_string() const;

  private:

    static std::regex WordSizeX;
    static std::map<Types, unsigned int> TypeSizeMap;
    static std::map<std::string, Types> StringTypeMap;
    static std::map<Types, std::string> TypeStringMap;

    unsigned int size = 0;
    Types type;

};

}

#endif
