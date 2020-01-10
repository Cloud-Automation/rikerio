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

    /**
     * @brief allocate memory
     * @params unsigned int size
     * @returns memory area
     */
    std::shared_ptr<MemoryArea> alloc(unsigned int);

    /**
     * @brief deallocate memory
     * @params unsigned int offset
     * @returns deallocated memory area object
     */
    std::shared_ptr<MemoryArea> dealloc(unsigned int);

    /**
     * @brief get associated allocated offset for a byte offset
     * @params byte offset
     * @returns allocated offset
     * @throws UnallocatedAreaException
     */
    unsigned int get_allocation_by_offset(unsigned int);

    /**
     * @brief return byte size
     * @returns byte size
     */
    unsigned int get_size() const;

    /**
     * @brief return memory area from range information
     * @params byte_offset
     * @params bit_size
     * @returns memory area object or nullptr
     */
    std::shared_ptr<MemoryArea> get_area_from_range(unsigned int, unsigned int);

    /**
     * @brief get memory area from a token
     * @params token
     * @returns memory area object or nullptr
     */
    std::shared_ptr<MemoryArea> get_area_from_token(const std::string&);

  private:

    Token token;

    unsigned int size;
    void* ptr;

    std::string filename;

    std::map<unsigned int, std::shared_ptr<MemoryArea>> allocMap;
    std::map<unsigned int, unsigned int> freeMap;
    std::map<const std::string, std::shared_ptr<MemoryArea>> tokenMap;

    void debug_print();

};


}



#endif
