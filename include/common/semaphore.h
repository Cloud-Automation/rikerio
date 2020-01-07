#ifndef __RIKERIO_CLIENT_SEMAPHORE_H__
#define __RIKERIO_CLIENT_SEMAPHORE_H__

#include "stdexcept"
#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/sem.h"
#include "string.h"

namespace RikerIO {

class Semaphore {
  public:

    class SemaphoreError : public std::exception {
      public:
        SemaphoreError() : message(strerror(errno)) {};
        const std::string& getMessage() {
            return message;
        }
      private:
        const std::string message;
    };

    Semaphore(int id);

    void lock();
    void unlock();

    int get_id();

  private:
    const int id;

};

}

#endif
