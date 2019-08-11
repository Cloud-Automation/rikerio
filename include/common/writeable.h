#ifndef __RIKERIO_COMMON_WRITEABLE_H__
#define __RIKERIO_COMMON_WRITEABLE_H__

namespace RikerIO {

class Writeable {
  public:
    virtual void write() = 0;
};

}

#endif
