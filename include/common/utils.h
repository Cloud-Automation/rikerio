#ifndef __RIKERIO_UTILS_H__
#define __RIKERIO_UTILS_H__

#include <string>
#include <map>
#include <vector>
#include <regex>

namespace RikerIO {

class Utils  {

  public:
    enum Datatype {
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

    static const std::vector<unsigned int> DatatypeSize;
    static std::regex wordSizeX;

    static std::string GetStringFromType(const Datatype& type);
    static Datatype GetTypeFromString(const std::string& str);

    static bool IsWordSize (const std::string& str);
    static unsigned int GetBitSize (const std::string& str);

};
}

#endif
