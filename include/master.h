#ifndef __RIO_MASTER_CONTAINER_H__
#define __RIO_MASTER_CONTAINER_H__

#include <map>
#include <memory>
#include <vector>

#define MASTER_TOKEN_SIZE 12

namespace RikerIO {

class Master {

  public:
    Master(std::string label, std::string token, int pid = -1) :
        id(Master::MasterCounter++), label(label), token(token), pid(pid) {

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

  private:

    static unsigned int MasterCounter;

    const unsigned int id;
    const std::string label;
    const std::string token;
    const int pid;

};


class MasterFactory {

  public:

    MasterFactory();

    Master& create(const std::string&, int);
    bool remove(const std::string&);
    bool remove(unsigned int);

    /*   Master& getMaster(std::string&);
        Master& getMaster(unsigned int);
    */

    std::vector<std::shared_ptr<Master>> list();

  private:

    static std::string TokenCharacterList;

    std::map<std::string, std::shared_ptr<Master>> tokenMap;
    std::map<unsigned int, std::shared_ptr<Master>> idMap;


};


}


#endif
