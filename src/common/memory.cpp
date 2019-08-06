#include "common/memory.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <grp.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "common/error.h"

using namespace RikerIO;

Memory::Memory(size_t size, std::string id) :
    size(size),
    id(id),
    ptr(NULL),
    filename("/var/run/rikerio/" + id + "/shm"),
    allocMap() {

    /* create file */

    int fd = open (filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    if (fd == -1) {
        throw InternalException(strerror(errno));
    }

    /* resize file */

    if (ftruncate(fd, size) == -1) {
        throw InternalException(strerror(errno));
    }

    /* get group */
    struct group* grp = getgrnam("rikerio");

    if (!grp) {
        throw InternalException("Error fetching rikerio system group.");
    }

    mode_t oldMask = umask(0);

    if (chown(filename.c_str(), -1, grp->gr_gid) != 0) {
        throw InternalException("Error setting group on shared memory file.");
    }

    if (chmod(filename.c_str(), 0664) != 0) {
        throw InternalException("Error setting permissions on shared memory file.");
    }

    umask(oldMask);

    close(fd);

    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


}

Memory::~Memory() {

    munmap(ptr, size);
    unlink(filename.c_str());

}
