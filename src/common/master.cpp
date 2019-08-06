#include "common/master.h"
#include "common/error.h"
#include <stdexcept>

using namespace RikerIO;

unsigned int Master::MasterCounter = 0;
std::string MasterFactory::TokenCharacterList = "abcdefghiklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

MasterFactory::MasterFactory() : tokenMap(), idMap()  {

}

Master& MasterFactory::create(const std::string& label, int pid = -1) {
    /* initialize random seed: */
    srand (time(NULL));
    std::string token = "";

    for (unsigned int tries = 0; tries < 3; tries += 1) {

        // create token

        token = "";
        for (unsigned int i = 0; i < MASTER_TOKEN_SIZE; i += 1) {
            /* generate secret number between 1 and 10: */
            unsigned int charPos = rand() % MasterFactory::TokenCharacterList.length();
            token.append(1, MasterFactory::TokenCharacterList.at(charPos));
        }

        if (tokenMap.find(token) == tokenMap.end()) {
            break;
        }

        if (tries == 2) {

            throw GenerateTokenException();

        }

    }

    std::shared_ptr<Master> m = std::make_shared<Master>(label, token, pid);
    tokenMap[m->getToken()] = m;
    idMap[m->getId()] = m;

    return *m;

}

bool MasterFactory::remove(const std::string& token) {

    if (tokenMap.find(token) == tokenMap.end()) {
        return false;
    }


    std::shared_ptr<Master> m = tokenMap[token];

    tokenMap.erase(token);
    idMap.erase(m->getId());

    return true;

}

bool MasterFactory::remove(const unsigned int id) {

    if (idMap.find(id) == idMap.end()) {
        return false;
    }


    std::shared_ptr<Master> m = idMap[id];

    idMap.erase(id);
    tokenMap.erase(m->getToken());

    return true;

}

std::vector<std::shared_ptr<Master>> MasterFactory::list() {

    std::vector<std::shared_ptr<Master>> result;

    for (auto m : idMap) {

        result.push_back(m.second);

    }

    return result;

}
