#if 0
#include "client/profile.h"
#include "config.h"
#include "common/error.h"

using namespace RikerIO::Client;

Profile::Profile(AbstractClient& client, std::string id = "default") :
    id(id),
    client(client),
    taskSet() {


}

Task& Profile::registerTask(std::string name, int pid, bool track) {

    std::string taskToken = client.task_register(name, pid, track);
    /* create temporary class so we can call private constructor with make_shared */
    class TmpTask : public Task {
      public:
        TmpTask(AbstractClient& c, std::string t) : Task(c, t) { }
    };

    auto task = std::make_shared<TmpTask>(client, taskToken);

    taskSet[task->getId()] = task;

    return *task;

}

void Profile::unregisterTask(Task& task) {

    client.task_unregister(task.getToken());

    taskSet.erase(task.getId());

}

std::set<std::string> Profile::getTaskList() {
    return client.task_list();
}


/*void Profile::dealloc(unsigned int offset) {

    if (taskToken == "") {
        throw InternalStateError();
    }

    if (allocMap.find(offset) == allocMap.end()) {
        throw InternalStateError();
    }

    client.memory_dealloc(offset, taskToken);

}

std::vector<std::shared_ptr<Allocation>> Profile::getLocalAlloc() {

    std::vector<std::shared_ptr<Allocation>> result;

    for (auto a : allocMap) {
        result.push_back(a.second);
    }

    return result;

}
*/

#endif
