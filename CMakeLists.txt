cmake_minimum_required (VERSION 3.2.0)
project (RikerIO CXX)
set (CMAKE_CXX_STANDARD 11)
include(ExternalProject)

option(BUILD_STATIC "Compile as static library."    OFF)
option(WITH_TESTS   "Build with tests"              OFF)
option(TARGET_ARCH  "Build for target"              "x86")

set(CMAKE_MODULE_PATH   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
set(SRC                 ${PROJECT_SOURCE_DIR}/src)
set(INC_BINARY          ${PROJECT_BINARY_DIR}/include)
set(INC                 ${PROJECT_SOURCE_DIR}/include)
set(TST                 ${PROJECT_SOURCE_DIR}/test)


if ("${TARGET_ARCH}" STREQUAL "arm")
    include(Toolchain-arm)
    set(CMAKE_TOOLCHAIN_FILE                ${CMAKE_MODULE_PATH}/Toolchain-arm.cmake)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE   "armhf")
endif()

if (${WITH_TESTS})
    #    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()


include(GetGitRevisionDescription)
include(version)
include(cpack)
include(jsoncpp)
include(json-rpc-cpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/rikerio/version.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/install.sh ${CMAKE_CURRENT_BINARY_DIR}/postinst)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/rikerio@.service ${CMAKE_CURRENT_BINARY_DIR}/rikerio@.service COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/rio-completion.bash ${CMAKE_CURRENT_BINARY_DIR}/rio-completion.bash COPYONLY)

include_directories(${PROJECT_BINARY_DIR}/include/rikerio)

# add rikerio library first

add_subdirectory(${SRC}/common)
add_subdirectory(${SRC}/server)
add_subdirectory(${SRC}/client)
add_subdirectory(${SRC}/cli)


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rikerio@.service DESTINATION /lib/systemd/system)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rio-completion.bash DESTINATION /etc/bash_completion.d)
#install(CODE "execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/install.sh ${CMAKE_INSTALL_PREFIX})")

