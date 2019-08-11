#ifndef __RIKERIO_LIB_H__
#define __RIKERIO_LIB_H__

#include "config.h"
#include "version.h"
#include "client/profile.h"

namespace RikerIO {

/*  class Data {
    public:
      Data(
        std::string id,
        unsigned int byteOffset,
        unsigned int bitOffset,
        unsigned int bitSize,
        Datatype type,
        Memory& memory);
      void read();
      void write();
    private:
      char* ptr;
      std::string id;
      unsigned int byteOffset;
      unsigned int bitOffset;
      unsigned int bitSize;
      Datatype type;
      Memory& memory;
  };

  class Allocation {
    public:
      Allocation(unsigned int offset, unsigned int size, std::string taskName);
    private:
      unsigned int offset;
      unsigned int size;
      std::string taskName;
  };

  class Memory {
    public:
      Memory(Profile&);
      char* getPointer();
    private:
      char* ptr;
  };

  class Semaphore {
    public:
      Semaphore(Profile&);
      bool lock();
      bool unlock();
    private:
      Profile& profile;
      sem_t sem;
  };


  class Task {
    public :
      Task(string token, Profile& profile);

      char* getMemoryPointer();
      unsigned int allocMemory(unsigned int size);
      bool deallocMemory(unsigned int offset);

      Data& createData(std::string id, unsigned int byteOffset, unsigned int bitOffset, unsigned int bitSize, Datatype type);
      bool removeData(std::string id);
      Data& getData(std::string& id);
      std::set<std::string> getDataUpdates();

      std::string& getToken();

    private:
      std::string token;
      Profile& profile;

  };

*/



}

#endif
