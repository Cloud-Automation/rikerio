#ifndef __RIKERIO_ABSTRACT_RESPONSE_H__
#define __RIKERIO_ABSTRACT_RESPONSE_H__

#include <string>
#include "common/error.h"

namespace RikerIO {

class AbstractResponse {
  public:
    virtual int get_code() = 0;
    virtual const std::string& get_message() = 0;

    bool ok() {
        return get_code() == RikerIO::NO_ERROR;
    }


};

}

#endif
