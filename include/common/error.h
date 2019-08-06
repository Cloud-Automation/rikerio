#ifndef __RIKERIO_ERROR_H__
#define __RIKERIO_ERROR_H__

#include <stdexcept>

namespace RikerIO {

enum Error {
    NO_ERROR = 0,
    UNAUTHORIZED_ERROR = 1,
    DUPLICATE_ERROR = 2,
    NOTFOUND_ERROR = 3,
    GENTOKEN_ERROR = 4
};

class GenerateTokenException : public std::runtime_error {
  public:
    GenerateTokenException() : runtime_error("") { }
};

class NotFoundException : public std::runtime_error {
  public:
    NotFoundException(): runtime_error("") { }
};

class InternalException : public std::runtime_error {
  public:
    InternalException(std::string msg) : runtime_error(msg.c_str())  { }
};

}


#endif
