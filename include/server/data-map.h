#ifndef __RIKERIO_DATA_MAP_H__
#define __RIKERIO_DATA_MAP_H__

#include <map>
#include <memory>
#include "server/data.h"
#include "server/memory.h"

namespace RikerIO {

class DataMap : public std::map<const std::string, std::shared_ptr<Data>> {

  public:
    DataMap(Memory&);

    /**
     * @brief Add data without a token. If such data without a token exists it will
     * be replaced. If such data with a token exists, nothing happens.
     * @param id
     * @param data pointer
     */
    bool add(const std::string, std::shared_ptr<Data>);

    /**
     * @brief Add data with a token. If such data already exists then replace it. If the existing
     * data is associated with a different token then nothing happens. If the existing data exists
     * without a token then it will be replaced
     * @param id
     * @param token
     * @param data pointer
     */
    bool add(const std::string, const std::string, std::shared_ptr<Data>);

    /**
     * @brief Remove a data point by its id. If the data point is associated with a
     * token then this will have not effect.
     * @param id
     * @returns true on removal
     */
    bool remove(const std::string&);

    /**
     * @brief Remove a data point by its id with a token. If the data point is associated with a
     * different/no token then this will have not effect.
     * @param id
     * @param token
     * @returns true on removal
     */
    bool remove(const std::string&, const std::string&);

    /**
     * @brief Remove all data associated by a token.
     * @param token
     * @returns number of removed data points
     */
    unsigned int removeByToken(const std::string&);

    /**
     * @brief Remove multiple data points located inside the
     * memory area from the arguments.
     * @param offset
     * @param size
     * @returns number of removed data points
     */
    unsigned int removeByRange(unsigned int, unsigned int);

    /**
     * @brief If there exist a token for that datapoint than
     * it is considered private.
     * @param id
     * @returns true if a token exists for that id
     */
    bool isPrivate(const std::string&);

    /**
     * @brief Returns the associated semaphore or -1
     * @param id
     * @returns semaphore id
     */
    int getSemaphore(const std::string&);

  private:

    Memory& memory;

    std::map<const std::string, std::string> dataTokenMap;
    std::map<const std::string, std::shared_ptr<MemoryArea>> dataMemoryMap;

};


}

#endif
