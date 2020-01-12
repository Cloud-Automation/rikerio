#include "server/memory.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <grp.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "common/error.h"

using namespace RikerIO;

Memory::Memory(unsigned int size) :
    token(TOKEN_LENGTH, TOKEN_MAX_TRIES),
    size(size) {

    freeMap.insert(std::pair<unsigned int, unsigned int>( 0, size ));

  
}

void Memory::debug_print() {

    for (auto f : freeMap) {
        printf("free %d (%d)\n", f.first, f.second);
    }

    for (auto f : allocMap) {
        printf("alloc %d (%d)\n", f.first, f.second->get_size());
    }


}

std::shared_ptr<MemoryArea> Memory::alloc(unsigned int size) {

    const std::string tkn = token.create();

    unsigned int toBeRemovedOffset = 0;
    unsigned int toBeRemovedSize = 0;
    bool inserted = false;

    std::shared_ptr<MemoryArea> ma = nullptr;

    for (auto f : freeMap) {
        if (f.second >= size) {
            ma = std::make_shared<MemoryArea>(tkn, f.first, size);
            allocMap[f.first] = ma;
            tokenMap[tkn] = ma;
            insert(ma);
            toBeRemovedOffset = f.first;
            toBeRemovedSize = f.second;
            inserted = true;
            break;
        }
    }

    if (!inserted) {
        throw OutOfSpaceError();
    }

    freeMap[toBeRemovedOffset + size] = toBeRemovedSize - size;
    freeMap.erase(toBeRemovedOffset);

    return ma;

}

std::shared_ptr<MemoryArea> Memory::dealloc(unsigned int offset) {

    // no harm done
    if (allocMap.find(offset) == allocMap.end()) {
        return nullptr;
    }

    std::shared_ptr<MemoryArea> result = allocMap[offset];

    // erase allocation
    allocMap.erase(offset);
    tokenMap.erase(result->get_token());
    erase(result);

    // we put the new, free memory area in the freeMap
    freeMap[offset] = result->get_size();

    // making optimizations to the free memory map
    // merging appending free memory areas

    unsigned int thingsDone = 0;
    unsigned int mergeOffset = 0;

    do {

        thingsDone = 0;

        // check appending free areas
        for (auto f : freeMap) {

            unsigned int fEnd = f.first + f.second;

            if (freeMap.find(fEnd) == freeMap.end()) {
                // no appendix found
                continue;
            }

            // appending found, remembering it
            thingsDone = 1;
            mergeOffset = f.first;

        }

        if (thingsDone > 0) {

            // erasing two separate free memory areas
            // and creating a new one
            unsigned int aOffset = mergeOffset;
            unsigned int aSize = freeMap[mergeOffset];
            unsigned int aEnd = mergeOffset + aSize;
            unsigned int bOffset = aEnd;
            unsigned int bSize = freeMap[bOffset];

            freeMap.erase(aOffset);
            freeMap.erase(bOffset);

            freeMap[aOffset] = aSize + bSize;

        }


    } while (thingsDone > 0);

    return result;

}

unsigned int Memory::get_size() const {

    return size;

}

std::shared_ptr<MemoryArea> Memory::get_area_from_range(unsigned int offset, unsigned int size) {

    std::shared_ptr<MemoryArea> result = nullptr;

    unsigned int end = offset + size;

    for (auto m : allocMap) {

        if (offset >= m.second->get_offset() && end <= m.second->get_end()) {
            result = m.second;
        }
    }

    return result;

}

std::shared_ptr<MemoryArea> Memory::get_area_from_token(const std::string& tkn) {

    if (tokenMap.find(tkn) == tokenMap.end()) {
        return nullptr;
    }

    return tokenMap[tkn];
}
