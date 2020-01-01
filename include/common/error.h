#ifndef __RIKERIO_ERROR_H__
#define __RIKERIO_ERROR_H__

#include <stdexcept>

namespace RikerIO {

enum Error {
    NO_ERROR = 0,
    UNAUTHORIZED_ERROR = -1,
    DUPLICATE_ERROR = -2,
    NOTFOUND_ERROR = -3,
    GENTOKEN_ERROR = -4,
    OUTOFSPACE_ERROR = -5,
    BAD_REQUEST = -6,
    INTERNAL_ERROR = -7
};

class GenerateTokenException : public std::runtime_error {
  public:
    GenerateTokenException() : runtime_error("") { }
};

class NotFoundException : public std::runtime_error {
  public:
    NotFoundException(): runtime_error("") { }
};

class ConnectionError : public std::runtime_error {
  public:
    ConnectionError() : runtime_error("Connection Error") {
    }
};

class InternalStateError : public std::runtime_error {
  public:
    InternalStateError() : runtime_error("Already registrated a Task on this client.") {}
};

class AllocationError : public std::runtime_error {
  public:
    AllocationError() : runtime_error("Error allocating memory.") { }
};

class PermissionError : public std::runtime_error {
  public:
    PermissionError() : runtime_error("This Client does not have permissions for this operation.") {}
};

class InternalError : public std::runtime_error {
  public:
    InternalError(std::string msg) : runtime_error(msg.c_str())  { }
};

class OutOfSpaceError : public std::runtime_error {
  public:
    OutOfSpaceError() : runtime_error("")  { }
};

class OutOfScopeError : public std::runtime_error {
  public:
    OutOfScopeError() : runtime_error("")  { }
};



}


#endif
