#ifndef __RIO_MEMORY_H__
#define __RIO_MEMORY_H__

#include <stdint.h>
#include <sys/types.h>
#include <string>
#include <map>
#include <set>

namespace RikerIO {

class Memory {

  public:
    /**
     * @param memory size
     * @param profile id
     */
    Memory(unsigned int, std::string);
    ~Memory();

    unsigned int alloc(unsigned int);
    bool dealloc(unsigned int);

    /**
     * @brief get associated allocated offset for a byte offset
     * @params byte offset
     * @returns allocated offset
     * @throws UnallocatedAreaException
     */
    unsigned int getAllocOffset(unsigned int);

  private:

    class Area {
      public:
        unsigned int offset;
        unsigned int size;
        friend bool operator<(const Area& lhs, const Area& rhs)  {
            return lhs.offset < rhs.offset;
        }
    };

    unsigned int size;
    std::string id;
    void* ptr;
    std::string filename;

    std::map<unsigned int, unsigned int> allocMap;
    std::set<Area> freeSet;

};


}



#endif
