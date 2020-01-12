#ifndef __RIKERIO_SHARED_MEMORY_H__
#define __RIKERIO_SHARED_MEMORY_H__

namespace RikerIO {

class SharedMemory {
public:
  
  SharedMemory(unsigned int size, std::string filename);
  ~SharedMemory();
  
  unsigned int get_size() const;

private:
  
  unsigned int size;
  void* ptr;
  std::string filename;

};


}

#endif