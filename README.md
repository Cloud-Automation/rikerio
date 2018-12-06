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

Master: A Master is a provider for IO Data. It also creates links and eventually aliasses. A Master writes and reads IO data between the actual hardware IO and a shared memory area.

Task: A Task operates on the IO Data provided by the Master.

Profile : A Profile describes a domain of common hardware IOs. For example a Modbus/TCP Master and a Hardware GPIO working for the same application. These Masters should operate on the same profile. A Profile contains a shared memory area, a semaphore to access the shared memory area, a profile lock to syncronize access to links and aliases, a memory allocation list, links and aliases.

Address: A address is the offset in a memory section provided by a profile. The address consists of a byte and bit offset.

Link: A Link is a string representation of one ore more addresses.

Alias: A Alias is a string representation for one ore more links.

The RikerIO Server Application creates a folder structure under /var/run/rikerio/{profile} and one under /var/lib/rikerio/{profile}. The profile, if not specified is called 'default'. Start the rio-server with the --id option and specifiy a specific profile name, for example:

`rio-server --id=io`

The /var/run/rikerio/{profile} folder is located on a tmpfs filesystem, so every action happening on those folders are performed in memory. When the RikerIO Server gets shutdown, the folder and most of its contests get destroyed. This folder holds the links, memory allocation list, semaphore, sync file and the shared memory file.

Try the following command to inspect the folder for yourself:

`ls /var/run/rikerio/default`

- alias: softlink to the folder /var/lib/rikerio/default/alias.
- links: folder containing the links.
- alloc: file with memory allocations.
- sem: file containing the semaphore id.
- shm: file containing the shared memory.
- sync: empty file for syncing link and alias operations.

The /var/lib/rikerio/{profile} folder is located on a permanent filesystem, so every action happening in this folder is stored on the systems harddrive.

The library operates on the single files with the flock and funlock system calls to synchronize multiple master or tasks working on a profile.

A user with the apropriate user rights can alter the content of the files but in order to not interfer with Master and Tasks this should not be done.

## API

Most of the operations happening on a profile can only be done through the RikerIO Library. Only operations on the alias can also be performed by a user via a command line interface.

The general pattern for receiving a list of objects is as follows:

- lock profile
- rio_object_count
- rio_object_get
- unlock profile

### Get number of Profiles

`int rio_profile_count(unsigned int* profileCount)`

*returns* Zero on success, -1 on failure.

### Get Profile list

`int rio_profile_get(rio_profile_t[])`

*returns* Zero on success, -1 on failure

### Add allocation

`int rio_alloc_add(rio_profile_t profile,
  uint32_t memorySize,
  char** memoryPointer,
  uint32_t* memoryOffset)`

*returns* Zero on success, -1 on failure

### Remove allocation

`int rio_alloc_rm(rio_profile_t profile, uint32_t memoryOffset)`

*returns* Zero on success, -1 on failure

### Remove all allocations

`int rio_alloc_rmall(rio_profile_t profile)`

*returns* Zero on success, -1 on failure

### Count allocations

`int rio_alloc_count(rio_profile_t profile, unsigned int* allocCount)`

*returns* Zero on success, -1 on failure

### Get allocations

`int rio_alloc_get(rio_profile_t profile, rio_alloc_entry_t list[])`

*returns* Zero on success, -1 on failure
