#ifndef __RIKERIO_CLIENT_TASK_H__
#define __RIKERIO_CLIENT_TASK_H__

#include <string>
#include <map>
#include <memory>
#include "client/abstract-client.h"
#include "client/allocation.h"
#include "client/data.h"

namespace RikerIO {

namespace Client {

class Profile;

class Task {
    friend class Profile;
  public:
    ~Task();

    Allocation& alloc(unsigned int size);
    void dealloc(Allocation&);

    void registerData(RikerIO::Data&, std::string id);
    void getData(std::string id);

    unsigned int getId();
    AbstractClient& getClient();
    std::string getToken();


  private:
    Task(AbstractClient&, std::string);

    static unsigned int localTaskCounter;

    unsigned int id;
    AbstractClient& client;
    std::string token;

    std::map<unsigned int, std::shared_ptr<Allocation>> allocMap;
};

}

}



#endif
