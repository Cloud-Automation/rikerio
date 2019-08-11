#ifndef __RIO_MASTER_CONTAINER_H__
#define __RIO_MASTER_CONTAINER_H__

#include <map>
#include <memory>
#include <vector>

#define TASK_TOKEN_SIZE 12

namespace RikerIO {

class Task {

  public:
    Task(std::string label, std::string token, int pid = -1, bool track = false) :
        id(++Task::TaskCounter), label(label), token(token), pid(pid), track(track) {

    }

    const int getId() {
        return id;
    }

    const std::string getLabel() {
        return label;
    }

    const std::string getToken() {
        return token;
    }

    const int getPid() {
        return pid;
    }

    const bool isTrack() {
        return track;
    }

  private:

    static unsigned int TaskCounter;

    const unsigned int id;
    const std::string label;
    const std::string token;
    const int pid;
    const bool track;

};


class TaskFactory {

  public:

    TaskFactory();

    Task& create(const std::string&, int, bool);
    bool remove(const std::string&);
    bool remove(unsigned int);

    std::shared_ptr<Task> operator[] (const unsigned int id) {
        return idMap[id];
    }

    std::shared_ptr<Task> operator[] (const std::string& token) {

        for (auto m : idMap) {

            if (m.second->getToken() == token) {
                return m.second;
            }

        }

        return nullptr;

    }


    std::vector<std::shared_ptr<Task>> list();

  private:

    static std::string TokenCharacterList;

    std::map<std::string, std::shared_ptr<Task>> tokenMap;
    std::map<unsigned int, std::shared_ptr<Task>> idMap;

};


}


#endif
