cmake_minimum_required (VERSION 3.2.0)
project (RikerIO CXX)
set (CMAKE_CXX_STANDARD 11)
include(ExternalProject)

option(BUILD_STATIC "Compile as static library."    OFF)
option(WITH_TESTS   "Build with tests"              ON)
option(TARGET_ARCH  "Build for target"              "x86")

set(CMAKE_MODULE_PATH   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
set(SRC                 ${PROJECT_SOURCE_DIR}/src)
set(INC                 ${PROJECT_BINARY_DIR}/include)
set(TST                 ${PROJECT_SOURCE_DIR}/test)


if ("${TARGET_ARCH}" STREQUAL "arm")
    include(Toolchain-arm)
    set(CMAKE_TOOLCHAIN_FILE                ${CMAKE_MODULE_PATH}/Toolchain-arm.cmake)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE   "armhf")
endif()

if (${WITH_TESTS})
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()


include(GetGitRevisionDescription)
include(version)
include(cpack)

include(jsoncpp)
include(json-rpc-cpp)

set(CMAKE_PREFIX_PATH ${JSONCPP_BIN_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/version.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/rikerio.h ${CMAKE_CURRENT_BINARY_DIR}/include/rikerio.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/install.sh ${CMAKE_CURRENT_BINARY_DIR}/postinst)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/rio.in ${CMAKE_CURRENT_BINARY_DIR}/rio COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/rikerio@.service ${CMAKE_CURRENT_BINARY_DIR}/rikerio@.service COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/rio-completion.bash ${CMAKE_CURRENT_BINARY_DIR}/rio-completion.bash COPYONLY)


file(GLOB HEADERS ${INC}/*.h)
file(GLOB SOURCES ${SRC}/*.c)

include_directories(${PROJECT_BINARY_DIR}/include)


# add rikerio library first

#[[
add_executable(rio-server
    ${SRC}/server/server.cpp
    ${SRC}/server/server-main.cpp
    ${SRC}/common/task.cpp
    ${SRC}/common/memory.cpp
    ${SRC}/common/data.cpp)

target_include_directories(rio-server PUBLIC ${JSONCPP_INC} ${JSONRPC_INC} ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR} )
target_link_libraries(rio-server rt dl ${JSONRPC_LIB_SERVER} ${JSONRPC_LIB_COMMON} ${JSONCPP_LIB} pthread)
add_dependencies(rio-server ${JSONCPP_PREFIX} ${JSONRPC_PREFIX})

add_executable(rio-task-register ${SRC}/cli/master-create.cpp)
target_include_directories(rio-task-register PUBLIC ${JSONCPP_INC} ${JSONRPC_INC} ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR} )
target_link_libraries(rio-task-register rt dl ${JSONRPC_LIB_CLIENT} ${JSONRPC_LIB_COMMON} ${JSONCPP_LIB} pthread)
add_dependencies(rio-task-register ${JSONCPP_PREFIX} ${JSONRPC_PREFIX})

add_executable(rio-task-unregister ${SRC}/cli/master-remove.cpp)
target_include_directories(rio-task-unregister PUBLIC ${JSONCPP_INC} ${JSONRPC_INC} ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR} )
target_link_libraries(rio-task-unregister rt dl ${JSONRPC_LIB_CLIENT} ${JSONRPC_LIB_COMMON} ${JSONCPP_LIB} pthread)
add_dependencies(rio-task-unregister ${JSONCPP_PREFIX} ${JSONRPC_PREFIX})

add_executable(rio-task-list ${SRC}/cli/master-list.cpp)
target_include_directories(rio-task-list PUBLIC ${JSONCPP_INC} ${JSONRPC_INC} ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR} )
target_link_libraries(rio-task-list rt dl ${JSONRPC_LIB_CLIENT} ${JSONRPC_LIB_COMMON} ${JSONCPP_LIB} pthread)
add_dependencies(rio-task-list ${JSONCPP_PREFIX} ${JSONRPC_PREFIX})

# rio memory alloc {--token=...} {size}
# rio memory dealloc {--token=...} {offset}
# rio memory inspect

add_executable(rio-data-add ${SRC}/cli/data-create.cpp)
target_include_directories(rio-data-add PUBLIC ${JSONCPP_INC} ${JSONRPC_INC} ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR} )
target_link_libraries(rio-data-add rt dl ${JSONRPC_LIB_CLIENT} ${JSONRPC_LIB_COMMON} ${JSONCPP_LIB} pthread)
add_dependencies(rio-data-add ${JSONCPP_PREFIX} ${JSONRPC_PREFIX})

# rio data remove/rm
# rio data show
# rio data list/ls
# rio data read
# rio data write
# rio data monitor

# rio link add {linkId} {dataId}
# rio link remove/rm {linkId + wildcard} | {linkId} {dataId}
# rio link show {linkId}
# rio link list/ls {pattern}
# rio link read {pattern.a} {pattern.b} ...
# rio link write {value} {pattern.a} {pattern.b} ...
# rio link monitor {pattern.a} {pattern.b} ...

#[[
install (TARGETS rikerio-lib rio-server rio-alias-add rio-alias-rm rio-alias-inspect
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
    RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rio DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rikerio@.service DESTINATION /lib/systemd/system)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rio-completion.bash DESTINATION /etc/bash_completion.d)
#install(CODE "execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/install.sh ${CMAKE_INSTALL_PREFIX})")
]]#
