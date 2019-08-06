#ifndef __RIKERIO_OWNER_FACTORY_H__
#define __RIKERIO_OWNER_FACTORY_H__

#include <map>
#include "common/error.h"

namespace RikerIO {

template <typename T, typename O>
class OwnerFactory {

  public:
    OwnerFactory() :
        dataOwnerMap(),
        ownerDataMap() {

    }

    bool assign(T subjectId, O ownerId) {

        if (dataOwnerMap.find(subjectId) != dataOwnerMap.end()) {
            return false;
        }

        dataOwnerMap[subjectId] = ownerId;

        ownerDataMap.insert(std::pair<O, T>(ownerId, subjectId));

        return true;

    }

    bool remove(T subjectId) {
        if (dataOwnerMap.find(subjectId) == dataOwnerMap.end()) {
            return false;
        }

        O ownerId = dataOwnerMap[subjectId];

        auto iterpair = ownerDataMap.equal_range(ownerId);

        auto it = iterpair.first;
        for (; it != iterpair.second; ++it) {
            if (it->second == subjectId) {
                ownerDataMap.erase(it);
                break;
            }
        }

    }

    O getOwner(T subjectId) {
        if (dataOwnerMap.find(subjectId) == dataOwnerMap.end()) {
            throw NotFoundException();
        }

        return dataOwnerMap[subjectId];

    }

  private:

    std::map<T, O> dataOwnerMap;
    std::multimap<O, T> ownerDataMap;


};


}


#endif
