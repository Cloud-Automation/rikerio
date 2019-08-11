#ifndef __RIKERIO_CLIENT_PROFILE_H__
#define __RIKERIO_CLIENT_PROFILE_H__

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "client/abstract-client.h"
#include "client/task.h"

namespace RikerIO {

namespace Client {

class Profile {
  public:

    Profile(AbstractClient& client, std::string profile);

    Task& registerTask(std::string name, int pid = -1, bool track = false);
    void unregisterTask(Task&);
    std::set<std::string> getTaskList();


  private:
    std::string id;
    AbstractClient& client;

    std::map<unsigned int, std::shared_ptr<Task>> taskSet;

};
}
}

#endif
