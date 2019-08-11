#include "common/task.h"
#include "common/error.h"
#include <stdexcept>

using namespace RikerIO;

unsigned int Task::TaskCounter = 0;
std::string TaskFactory::TokenCharacterList = "abcdefghiklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

TaskFactory::TaskFactory() : tokenMap(), idMap()  {

}

Task& TaskFactory::create(const std::string& label, int pid = -1, bool track = false) {
    /* initialize random seed: */
    srand (time(NULL));
    std::string token = "";

    for (unsigned int tries = 0; tries < 3; tries += 1) {

        // create token

        token = "";
        for (unsigned int i = 0; i < TASK_TOKEN_SIZE; i += 1) {
            /* generate secret number between 1 and 10: */
            unsigned int charPos = rand() % TaskFactory::TokenCharacterList.length();
            token.append(1, TaskFactory::TokenCharacterList.at(charPos));
        }

        if (tokenMap.find(token) == tokenMap.end()) {
            break;
        }

        if (tries == 2) {

            throw GenerateTokenException();

        }

    }

    std::shared_ptr<Task> m = std::make_shared<Task>(label, token, pid);
    tokenMap[m->getToken()] = m;
    idMap[m->getId()] = m;

    return *m;

}

bool TaskFactory::remove(const std::string& token) {

    if (tokenMap.find(token) == tokenMap.end()) {
        return false;
    }


    std::shared_ptr<Task> m = tokenMap[token];

    tokenMap.erase(token);
    idMap.erase(m->getId());

    return true;

}

bool TaskFactory::remove(const unsigned int id) {

    if (idMap.find(id) == idMap.end()) {
        return false;
    }


    std::shared_ptr<Task> m = idMap[id];

    idMap.erase(id);
    tokenMap.erase(m->getToken());

    return true;

}

std::vector<std::shared_ptr<Task>> TaskFactory::list() {

    std::vector<std::shared_ptr<Task>> result;

    for (auto m : idMap) {

        result.push_back(m.second);

    }

    return result;

}
