#include "common/semaphore.h"

RikerIO::Semaphore::Semaphore(int id) : id(id) {

}

void RikerIO::Semaphore::lock() {

    struct sembuf semaphore = { };

    semaphore.sem_op = -1;
    semaphore.sem_flg = SEM_UNDO;
    if (semop(id, &semaphore, 1) == -1) {
        throw SemaphoreError();
    }

}

void RikerIO::Semaphore::unlock() {

    struct sembuf semaphore = { };

    semaphore.sem_op = 1;
    semaphore.sem_flg = SEM_UNDO;
    if (semop(id, &semaphore, 1) == -1) {
        throw SemaphoreError();
    }

}

int RikerIO::Semaphore::get_id() {
    return id;
}
