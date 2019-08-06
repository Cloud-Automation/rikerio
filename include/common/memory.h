#ifndef __RIO_MEMORY_H__
#define __RIO_MEMORY_H__

#include <stdint.h>
#include <sys/types.h>
#include <string>
#include <map>

namespace RikerIO {

class Memory {

  public:
    /**
     * @param memory size
     * @param profile id
     */
    Memory(size_t, std::string);
    ~Memory();

    unsigned int alloc(size_t);
    bool dealloc(unsigned int);

    /**
     * @brief get associated allocated offset for a byte offset
     * @params byte offset
     * @returns allocated offset
     * @throws UnallocatedAreaException
     */
    unsigned int getAllocOffset(unsigned int);

  private:

    size_t size;
    std::string id;
    void* ptr;
    std::string filename;

    std::map<unsigned int, size_t> allocMap;

};


}



#endif
