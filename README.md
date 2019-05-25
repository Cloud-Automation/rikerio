# RikerIO

RikerIO is a small framework that helps connecting IO Systems like General Purpose IOs or Bussystems easily with your application task.

![RikerIO Sketch](doc/sketch.png)

## Benefits

- No need to implement the IO System into your Application.
- Separate IO and Application.
- Build Applications with a PDO interface nearly independet of the IO System.
- Easily test and simulate your application.

## Platform

RikerIO is developed to be used with any modern linux based Operating System.

## Components

RikerIO contains the following entities:
- Library
- Command Line Tools

## Names and Conventions

**Master** : A Master is a provider for IO Data. It also creates links and eventually aliasses. A Master writes and reads IO data between the actual hardware IO and a shared memory area.

**Task** : A Task operates on the IO Data provided by the Master.

**Profile** : A Profile describes a domain of common hardware IOs. For example a Modbus/TCP Master and a Hardware GPIO working for the same application. These Masters should operate on the same profile. A Profile contains a shared memory area, a semaphore to access the shared memory area, a profile lock to syncronize access to links and aliases, a memory allocation list, links and aliases.

**Addresse** : A address is the offset in a memory section provided by a profile. The address consists of a byte and bit offset.

**Link** : A Link is a string representation of one ore more addresses.

**Alias** : A Alias is a string representation for one ore more links.

The RikerIO CLI creates a folder structure under /var/run/rikerio/{profile} and one under /var/lib/rikerio/{profile}. The profile, if not specified is called 'default'. Start the rio-server with the --id option and specifiy a specific profile name, for example:

```
rio profile create --id=io
```

This call is a long running server application holding the shared memory. This should be run as a daemon application with systemd for example.

The /var/run/rikerio/{profile} folder is located on a tmpfs filesystem, so every action happening on those folders are performed in memory. When the RikerIO CLI gets shutdown, the folder and most of its contests get destroyed. This folder holds the links, memory allocation list, semaphore, sync file and the shared memory file.

Try the following command to inspect the folder for yourself:

```
ls /var/run/rikerio/io
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
cmake -DCMAKE_BUILD_TYPE=Release ../..
make
```

In that case the RikerIO Server notifies SystemD when it is ready. Make sure to declare the service type as notify.

## Install

Install the application by typing
```
sudo make install
```
from the build directory.

CMake specifies the installation prefix as /usr/local. On some systems that can lead to problems (Ubuntu for example). Add `-DCMAKE_INSTALL_PREFIX=/usr/` to the cmake command to specify an alternate installation prefix.


## CLI

Type `rio help` to get the basic CLI Commands for RikerIO. From the Command Line you can create a new profile, create, alter and remove aliases and inspect links and aliases. Links should be created from a Master application.

## API

Most of the operations happening on a profile can only be done through the RikerIO Library. Only operations on the alias can also be performed by a user via a command line interface.

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
int rio_profile_get(rio_profile_t[] profileList, unsigned int listSize, unsigned int* retSize)`
```

**Arguments and Return Value**
- **profileList** Preallocated list of profiles, will be filled by the lib.
- **listSize** Item count of the preallocated list.
- **retSize** Actual count of items copied to the list.
- **returns** Zero on success, -1 on failure.

**Example**

```
unsigned int profileCount = 0;
if (rio_profile_count(&profileCount) == -1) {
    exit(EXIT_FAILURE);
}

rio_profile_t* list = calloc(profileCount, sizeof(rio_profile_t));

unsigned int retSize = 0;
if (rio_profile_get(list, profileCount, &retSize) == -1) {
  free(list);
  exit(EXIT_FAILURE);
}

for (unsigned int i = 0; i < retSize; i += 1) {
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
int rio_alloc_get(
  rio_profile_t profile,
  rio_alloc_entry_t list[],
  unsigned int listSize,
  unsigned int* retSize)
```

**Arguments and Return Value**
- **profile** Profile ID
- **list** Preallocated List of Allocations.
- **listSize** Item count for the preallocated list.
- **retSize** List size of actual list size filled by the library.
- **returns** Zero on success, -1 on failure

**Example**
```

unsigned int allocCount = 0;
if (rio_alloc_count("default", &allocCount == -1) {
    exit(EXIT_FAILURE);
}

rio_alloc_entry_t list[] = calloc(allocCount, sizeof(rio_alloc_entry));
unsigned int retSize = 0;
if (rio_alloc_get("default", list, allocCount, &retSize) == -1) {
  free(list);
  exit(EXIT_FAILURE);
}

for (unsigned int i = 0; i < retSize; i += 1) {
  printf("Offset %d, Size %d\n", list[i].offset, list[i].size);
}
free(list);

```

### Links

#### Address

```
typedef struct __rio_adr_t {
  uint32_t byteOffset;
  uint8_t bitOffset;
} rio_adr_t;
```

#### Add Address to a Link

**Signature**
```
int rio_link_adr_add(rio_profile_t profile, rio_link_t link, rio_adr_t adr)
```

**Arguments and Return Value**
- **profile** Profile ID
- **link** Linkname
- **adr** Address to be added to the link.
- **returns** Zero on success, -1 on failure.

**Example**
```
rio_adr_t adr = { 10, 2 };
if (rio_link_adr_add("default", "in.some.var", adr) == -1) {
  exit(EXIT_FAILURE);
}
```

#### Count Addresses associated with a Link

**Signature**
```
int rio_link_adr_count(rio_profile_t profile, rio_link_t link, unsigned int* adrCount)
```

**Arguments and Return Value**
- **profile** Profile ID
- **link** Linkname
- **adrCount** Pointer to Address Count result.
- **returns** Zero on success, -1 on failure.

**Example**
```
unsigned int adrCount = 0;
if (rio_link_adr_count("default", "out.another.var", &adrCount) == -1) {
  exit(EXIT_FAILURE);
}
printf("%d\n", adrCount);
```

#### Get Address List from a Link

**Signature**
```
int rio_link_adr_get(
  rio_profile_t profile,
  rio_link_t link,
  rio_adr_t[] adrList,
  unsigned int listSize,
  unsigned int* retSize)
```

**Arguments and Return Value**
- **profile** Profile ID
- **link** Linkname
- **adrList** Preallocated List of Addresses to be filled by the Library.
- **listSize** Item count for the preallocated list.
- **retSize** actual Size of the copied list items.
- **returns** Zero on success, -1 on failure.

**Example**

```

unsigned int adrCount = 0;
if (rio_link_adr_count("default", "in.foo.bar", &adrCount) == -1) {
  exit(EXIT_FAILURE);
}

rio_adr_t* adrList = calloc(adrCount, sizeof(rio_adr_t));
unsigned int retSize = 0;
if (rio_link_adr_get("default", "in.foo.bar", adrList, adrCount, &retSize) == -1) {
  free(adrList);
  exit(EXIT_FAILURE);
}

for (unsigned int i = 0; i < retSize; i += 1) {
  printf("%s : %d.%d\n", "in.foo.bar", adrList[i].byteOffset, adrList[i].bitOffset);
}

```

#### Remove Address from a Link

**Signature**
```
int rio_link_adr_rm(rio_profile_t profile, rio_link_t link, rio_adr_t adr)
```

**Arguments and Return Value**
- **profile** Profile ID
- **link** Linkname
- **adr** Address to be removed from the link.
- **returns** Zero on success, -1 on failure.

**Example**
```
rio_adr_t adr = { 10, 1 };
if (rio_link_adr_rm("default", "out.foo.bar", adr) == -1) {
  exit(EXIT_FAILURE);
}
```

#### Count Links in a Profile

**Signature**
```
int rio_link_count(rio_profile_t profile, unsigned int* linkCount)
```

**Arguments and Return Value**
- **profile** Profile ID
- **linkCount** Point to a variable where the application can store the result.
- **returns** Zero on success, -1 on failure.

**Example**
```
unsigned int linkCount = 0;
if (rio_link_count("default", &linkCount) == -1) {
  exit(EXIT_FAILURE);
}
printf("%d\n", linkCount);
```

#### Get Links in a Profile

**Signature**
```
int rio_link_get(
  rio_profile_t profile,
  rio_link_t[] linkList,
  unsigned int preAllocCount,
  unsigned int* listCount)
```

**Arguments and Return Value**
- **profile** Profile ID
- **linkList** Preallocated List where the results will be stored in.
- **preAllocCount** Number of preallocated list items.
- **listCount** Number of links copied to the list.
- **returns** Zero on success, -1 on failure.

**Example**

```
unsigned int fetchCount = 0;
rio_link_t* linkList = calloc(100, sizeof(rio_link_t));
if (rio_link_get("default", linkList, 100, &fetchCount) == -1) {
  exit(EXIT_FAILURE);
}
```

#### Remove Link

**Signature**
```
int rio_link_rm(
  rio_profile_t profile,
  rio_link_t link)
```

**Arguments and Return Value**
- **profile** Profile ID
- **link** Link to be removed.
- **returns** Zero on success, -1 on failure.

**Example**

```

if (rio_link_rm("default", "somelink") == -1) {
  exit(EXIT_FAILURE);
}
```

#### Remove all Links

**Signature**
```
int rio_link_rmall(
  rio_profile_t profile)
```

**Arguments and Return Value**
- **profile** Profile ID
- **returns** Zero on success, -1 on failure.

**Example**

```

if (rio_link_rmall("default") == -1) {
  exit(EXIT_FAILURE);
}
```

#### Add Link to Alias

**Signature**
```
int rio_alias_link_add(
  rio_profile_t profile,
  rio_alias_t alias,
  rio_link_t)
```

**Arguments and Return Value**
- **profile** Profile ID
- **alias** Alias to operate on.
- **link** Link to be added to the alias.
- **returns** Zero on success, -1 on failure.

**Example**

```

if (rio_alias_link_add("default", "somealias", "somelink") == -1) {
  exit(EXIT_FAILURE);
}
```

#### Remove Link from Alias

**Signature**
```
int rio_alias_link_rm(
  rio_profile_t profile,
  rio_alias_t alias,
  rio_link_t link)
```

**Arguments and Return Value**
- **profile** Profile ID
- **alias** Alias to operate on.
- **link** Link to be removed.
- **returns** Zero on success, -1 on failure.

**Example**

```

if (rio_alias_link_rm("default", "somealias", "somelink") == -1) {
  exit(EXIT_FAILURE);
}
```

#### Count addresses in alias

**Signature**
```
int rio_alias_adr_count(
  rio_profile_t profile,
  rio_alias_t alias,
  unsigned int* adrCount)
```

**Arguments and Return Value**
- **profile** Profile ID
- **alias** Alias
- **adrCount** Point to the count result.
- **returns** Zero on success, -1 on failure.

**Example**

```

unsigned int count = 0;
if (rio_alias_adr_count("default", "somealias", &count) == -1) {
  exit(EXIT_FAILURE);
}
```

#### Get Adresse List from Alias

**Signature**
```
int rio_alias_adr_get(
  rio_profile_t profile,
  rio_alias_t alias,
  rio_adr_t[] adrList,
  unsigned int preAdrCount,
  unsigned int* listSize)
```

**Arguments and Return Value**
- **profile** Profile ID
- **alias** Alias
- **adrList** List where the Addresses are copied to.
- **preAdrCount** Expected Addresse Count.
- **listSize** Delivered Addresse Count.
- **returns** Zero on success, -1 on failure.

**Example**

```

unsigned int preAdrCount = 0;

if (rio_alias_adr_count("deault", "somealias", &preAdrCount) == -1) {
  exit(EXIT_FAILURE);
}

unsigned int listSize = 0;
rio_adr_t* list = calloc(preAdrCount, sizeof(rio_adr_t));

if (rio_alias_adr_get("default", "somealias", &list, preAdrCount, &listSize) == -1) {
  exit(EXIT_FAILURE);
}
```

#### Get Profile Semaphore

**Signature**
```
int rio_sem_get(rio_profile_t profile, int* semId)
```

**Arguments and Return Value**
- **profile** Profile ID
- **semId** Semaphore ID pointer
- **returns** Zero on success, -1 on failure.

**Example**

```

int semId = -1;

if (rio_sem_get("default", &semId) == -1) {
  exit(EXIT_FAILURE);
}
```

#### Lock Semaphore

**Signature**
```
int rio_sem_lock(int semId)
```

**Arguments and Return Value**
- **semId** Semaphore ID
- **returns** Zero on success, -1 on failure.

**Example**

```

if (rio_sem_lock(semId) == -1) {
  exit(EXIT_FAILURE);
}
```

#### Unlock Semaphore

**Signature**
```
int rio_sem_unlock(int semId)
```

**Arguments and Return Value**
- **semId** Semaphore ID
- **returns** Zero on success, -1 on failure.

**Example**

```

if (rio_sem_unlock(semID) == -1) {
  exit(EXIT_FAILURE);
}
```

## License
LGPLv3  
