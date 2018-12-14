# RikerIO

RikerIO is a small framework that helps connecting IO Systems like General Purpose IOs or Bussystems easily with your application task.

## Platform

RikerIO is developed to be used with any modern linux based Operating System.

## Components

RikerIO contains the following entities:
- Server Application
- Library
- Command Line Tools

## Names and Conventions

**Master** : A Master is a provider for IO Data. It also creates links and eventually aliasses. A Master writes and reads IO data between the actual hardware IO and a shared memory area.

**Task** : A Task operates on the IO Data provided by the Master.

**Profile** : A Profile describes a domain of common hardware IOs. For example a Modbus/TCP Master and a Hardware GPIO working for the same application. These Masters should operate on the same profile. A Profile contains a shared memory area, a semaphore to access the shared memory area, a profile lock to syncronize access to links and aliases, a memory allocation list, links and aliases.

**Addresse** : A address is the offset in a memory section provided by a profile. The address consists of a byte and bit offset.

**Link** : A Link is a string representation of one ore more addresses.

**Alias** : A Alias is a string representation for one ore more links.

The RikerIO Server Application creates a folder structure under /var/run/rikerio/{profile} and one under /var/lib/rikerio/{profile}. The profile, if not specified is called 'default'. Start the rio-server with the --id option and specifiy a specific profile name, for example:

```
rio-server --id=io
```

The /var/run/rikerio/{profile} folder is located on a tmpfs filesystem, so every action happening on those folders are performed in memory. When the RikerIO Server gets shutdown, the folder and most of its contests get destroyed. This folder holds the links, memory allocation list, semaphore, sync file and the shared memory file.

Try the following command to inspect the folder for yourself:

```
ls /var/run/rikerio/default
```

- alias: softlink to the folder /var/lib/rikerio/default/alias.
- links: folder containing the links.
- alloc: file with memory allocations.
- sem: file containing the semaphore id.
- shm: file containing the shared memory.
- sync: empty file for syncing link and alias operations.

The /var/lib/rikerio/{profile} folder is located on a permanent filesystem, so every action happening in this folder is stored on the systems harddrive.

The library operates on single files with the *flock* and *funlock* system calls to synchronize multiple master or tasks working on a profile.

A user with the apropriate user rights can alter the content of the files but in order to not interfer with Master and Tasks this should not be done.

## Build

The Library is build using CMake, simply do the following:
```
mkdir build
cd build
cmake ../..
make
```

In case you want to use the RikerIO Server with systemd, add the SystemD Option to the cmake command:
```
cmake -DWITH_SYSTEMD=ON
```
In that case the RikerIO Server notifies SystemD when it is ready. Make sure to declare the service type as notify.

## API

Most of the operations happening on a profile can only be done through the RikerIO Library. Only operations on the alias can also be performed by a user via a command line interface.

The general pattern for receiving a list of objects is as follows:

1. lock profile
2. rio_object_count
3. rio_object_get
4. unlock profile

### Profiles

#### Get number of profiles

**Signature**
```
int rio_profile_count(unsigned int* profileCount)
```

**Arguments and Return Value**
- **profileCount** Return value for the number of profiles.
- **returns** Zero on success, -1 on failure.

**Example**
```
int retVal = 0;
unsigned int profileCount = 0;
retVal = rio_profile_count(&profileCount);
if (retVal == -1) {
    exit(EXIT_FAILURE);
}
printf("%d\n", profileCount);
```

#### Get Profile list

**Signature**
```
int rio_profile_get(rio_profile_t[] profileList)`
```

**Arguments and Return Value**
- **profileList** Preallocated list of profiles, will be filled by the lib.
- **returns** Zero on success, -1 on failure.

**Example**

```
unsigned int profileCount = 0;
if (rio_profile_count(&profileCount) == -1) {
    exit(EXIT_FAILURE);
}

rio_profile_t* list = calloc(profileCount, sizeof(rio_profile_t));

if (rio_profile_get(list) == -1) {
  free(list);
  exit(EXIT_FAILURE);
}

for (unsigned int i = 0; i < profileCount; i += 1) {
  printf("%s\n", list[i]);
}
free(list);
```

### Allocations
#### Add allocation

**Signature**
```
int rio_alloc_add(
  rio_profile_t profile,
  uint32_t memorySize,
  char** memoryPointer,
  uint32_t* memoryOffset)`
```
**Arguments and Return Value**
- **profile** Profile ID
- **memorySize** desired memory size
- **memoryPointer** Pointer to the pointer for the shared memory area.
- **memoryOffset** Pointer the value for the memory offset.
- **returns** Zero on success, -1 on failure

**Example**
```
char* memoryPointer = NULL;
uint32_t memoryOffset = 0;
if (rio_alloc_add("default", 128, &memoryPointer, &memoryOffset) == -1) {
  exit(EXIT_FAILURE);
}
```

#### Remove allocation

**Signature**
```
int rio_alloc_rm(rio_profile_t profile, uint32_t memoryOffset)
```
**Arguments and Return Value**
- **profile** Profile ID
- **memoryOffset** Memory offset to be released.
- **returns** Zero on success, -1 on failure

**Example**
```
char* memoryPointer = NULL;
uint32_t memoryOffset = 0;
if (rio_alloc_add("default", 128, &memoryPointer, &memoryOffset) == -1) {
  exit(EXIT_FAILURE);
}

if (rio_alloc_rm("default", memoryOffset) == -1) {
  exit(EXIT_FAILURE);
}
```

#### Remove all allocations

**Signature**
```
int rio_alloc_rmall(rio_profile_t profile)
```

**Arguments and Return Value**
- **profile** Profile ID
- **returns** Zero on success, -1 on failure

**Example**
```
if (rio_alloc_rmall("default") == -1) {
  exit(EXIT_FAILURE);
}
```

#### Count allocations

**Signature**
```
int rio_alloc_count(rio_profile_t profile, unsigned int* allocCount)
```

**Arguments and Return Value**
- **profile** Profile ID
- **allocCount** Pointer to allocation count variable. Will be filled by the application.
- **returns** Zero on success, -1 on failure

**Example**
```
unsigned int allocCount = 0;
if (rio_alloc_count("default", &allocCount) == -1) {
  exit(EXIT_FAILURE);
}
printf("%d\n", allocCount);
```

#### Get allocations

**Signature**
```
int rio_alloc_get(rio_profile_t profile, rio_alloc_entry_t list[])
```

**Arguments and Return Value**
- **profile** Profile ID
- **list** Preallocated List of Allocations.
- **returns** Zero on success, -1 on failure

**Example**
```
int lock = 0;
if (rio_lock("default", &lock) == -1) {
  exit(EXIT_FAILURE);
}

unsigned int allocCount = 0;
if (rio_alloc_count("default", &allocCount == -1) {
    exit(EXIT_FAILURE);
}

rio_alloc_entry_t list[] = calloc(allocCount, sizeof(rio_alloc_entry));
if (rio_alloc_get("default", list) == -1) {
  free(list);
  exit(EXIT_FAILURE);
}

for (unsigned int i = 0; i < allocCount; i += 1) {
  printf("Offset %d, Size %d\n", list[i].offset, list[i].size);
}
free(list);

if (rio_unlock(lock) == -1) {
  exit(EXIT_FAILURE);
}
```
