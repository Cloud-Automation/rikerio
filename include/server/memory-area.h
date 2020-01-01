#ifndef __RIKERIO_MEMORY_AREA_H__
#define __RIKERIO_MEMORY_AREA_H__

#include <sys/sem.h>
#include <memory>

namespace RikerIO {

class MemoryArea {

  public:

    MemoryArea(const std::string token, unsigned int offset, unsigned int size) :
        token(token),
        offset(offset),
        size(size),
        semaphore(0) {
        semaphore = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0664);
    }

    ~MemoryArea() {
        semctl(semaphore, -1, IPC_RMID);
    }

    const std::string& getToken() {
        return token;
    }

    friend bool operator<(const MemoryArea& lhs, const MemoryArea& rhs)  {
        return lhs.offset < rhs.offset;
    }

    unsigned int getOffset() const {
        return offset;
    }

    unsigned int getSize() const {
        return size;
    }

    int getSemaphore() const {
        return semaphore;
    }

    unsigned int getEnd() const {
        return offset + size;
    }

  private:

    const std::string token;
    const unsigned int offset;
    const unsigned int size;
    int semaphore;
};

using MemoryAreaPtr = std::shared_ptr<MemoryArea>;



}

#endif
