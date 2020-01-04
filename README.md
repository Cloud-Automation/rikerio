# RikerIO

RikerIO is a framework that helps connecting IO Systems like General Purpose IOs or Bussystems easily with your application task.

![RikerIO Sketch](doc/sketch.png)

## Benefits

- No need to implement the IO System into your Application.
- Separate IO and Application.
- Build Applications with a PDO interface nearly independent of the IO System.
- Easily test and simulate your application.

## Platform

RikerIO is developed to be used with any modern linux based Operating System.

## Components

RikerIO contains the following entities:
- Server
- Library (C++11)
- Command Line Tools

## Names and Conventions

**Master** : A Master is a provider for IO Data. It also creates links and eventually aliasses. A Master writes and reads IO data between the actual hardware IO and a shared memory area.

**Task** : A Task operates on the IO Data provided by the Master.

**Profile** : A Profile describes a domain of common hardware IOs. For example a Modbus/TCP Master and a Hardware GPIO working for the same application. These Masters should operate on the same profile. A Profile contains a shared memory area, a semaphore to access the shared memory area, a profile lock to syncronize access to links and aliases, a memory allocation list, links and aliases.

**Data** : A address is the offset in a memory section provided by a profile. The address consists of a byte and bit offset.

**Link** : A Link is a string representation of one ore more addresses.

## Build

The Library is build using CMake, simply do the following:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../..
make libjsonrpccpp
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

Type `rio --help` to get the basic CLI Commands for RikerIO. From the Command Line you can allocate memory, create, alter, list and remove data and links.


## License
LGPLv3  
