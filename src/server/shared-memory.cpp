#include "server/shared-memory.h"


RikerIO::SharedMemory::SharedMemory(unsigned int size, std::string filename) : 
size(size),
ptr(NULL),
filename(filename) {

  /* create file */

    int fd = open (filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    if (fd == -1) {
        throw InternalError(strerror(errno));
    }

    /* resize file */

    if (ftruncate(fd, size) == -1) {
        throw InternalError(strerror(errno));
    }

    /* get group */
    struct group* grp = getgrnam("rikerio");

    if (!grp) {
        throw InternalError("Error fetching rikerio system group.");
    }

    mode_t oldMask = umask(0);

    if (chown(filename.c_str(), -1, grp->gr_gid) != 0) {
        throw InternalError("Error setting group on shared memory file.");
    }

    if (chmod(filename.c_str(), 0664) != 0) {
        throw InternalError("Error setting permissions on shared memory file.");
    }

    umask(oldMask);


    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if ((void*) -1 == ptr) {
        throw InternalError(strerror(errno));
    }

    if (mlockall(MLOCK_ONFAULT) != 0) {
        throw InternalError(strerror(errno));
    }

    close(fd);

}

RikerIO::SharedMemory::~SharedMemory() {
  
  munlockall();

  munmap(ptr, size);
  unlink(filename.c_str());

}

unsigned int RikerIO::SharedMemory::get_size() const {
  return size;
}