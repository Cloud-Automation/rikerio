#include "client/allocation.h"
#include "client/task.h"

using namespace RikerIO::Client;

Allocation::Allocation(unsigned int offset, unsigned int size) :
    offset(offset), size(size), runningOffset(0), dataSet() {
}

unsigned int Allocation::getOffset() {
    return offset;
}
unsigned int Allocation::getSize() {
    return size;
}
