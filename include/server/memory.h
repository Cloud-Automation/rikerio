#ifndef __RIO_MEMORY_H__
#define __RIO_MEMORY_H__

#include <stdint.h>
#include <sys/types.h>

#include <string>
#include <map>
#include <set>
#include <memory>

#include "server/memory-area.h"
#include "server/token.h"

#define TOKEN_MAX_TRIES 100
#define TOKEN_LENGTH 12

namespace RikerIO {

class Memory : public std::set<std::shared_ptr<MemoryArea>> {

  public:
    /**
     * @param memory size
     * @param profile id
     */
    Memory(unsigned int, std::string);
    ~Memory();

    std::shared_ptr<MemoryArea> alloc(unsigned int);
    std::shared_ptr<MemoryArea> dealloc(unsigned int);

    /**
     * @brief get associated allocated offset for a byte offset
     * @params byte offset
     * @returns allocated offset
     * @throws UnallocatedAreaException
     */
    unsigned int getAllocOffset(unsigned int);

    unsigned int getSize() const;

    std::shared_ptr<MemoryArea> getAreaFromRange(unsigned int, unsigned int);
    std::shared_ptr<MemoryArea> getAreaFromToken(const std::string&);

  private:

    Token token;

    unsigned int size;
    void* ptr;

    std::string filename;

    std::map<unsigned int, std::shared_ptr<MemoryArea>> allocMap;
    std::map<unsigned int, unsigned int> freeMap;
    std::map<const std::string, std::shared_ptr<MemoryArea>> tokenMap;

    void debugPrint();

};


}



#endif
