cmake_minimum_required (VERSION 3.2.0)
project (RikerIO C)

enable_testing()

option(BUILD_STATIC "Compile as static library." OFF)
option(WITH_TESTS "Build with tests" OFF)
option(TARGET_ARCH "Build for target" "x86")


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

if ("${TARGET_ARCH}" STREQUAL "arm")
    include(Toolchain-arm)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/Toolchain-arm.cmake)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")
endif()


include(ExternalProject)
include(GetGitRevisionDescription)

git_describe(VERSION --tags --dirty=-d)

# parse the version information into pieces.
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")

set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")



# CPack

set(CPACK_PACKAGE_NAME "rikerio")
set(CPACK_PACKAGE_VENDOR "Cloud Automation")
set(CPACK_PACKAGE_VERSION_MAJOR VERSION_MAJOR)
set(CPACK_PACKAGE_VERSION_MINOR VERSION_MINOR)
set(CPACK_PACKAGE_VERSION_PATCH VERSION_PATCH)
set(CPACK_PACKAGE_EXECUTABLES "rio librio.so")

SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RikerIO Memory Server and Library.")
SET(CPACK_PACKAGE_VENDOR "Cloud Automation")
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
SET(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
SET(CPACK_PACKAGE_CONTACT "Stefan Pöter <stage-automation@cloud-automation.de>")

#set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libyaml-dev")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

include(CPack)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/version.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/rikerio.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/rikerio.h)

set(SRC ${PROJECT_SOURCE_DIR}/src)
set(INC ${PROJECT_BINARY_DIR}/include)
set(TST ${PROJECT_SOURCE_DIR}/test)


FILE(GLOB HEADERS ${INC}/*.h)
FILE(GLOB SOURCES ${SRC}/*.c)

add_executable(rikerio-bin ${SRC}/main.c)
set_target_properties(rikerio-bin PROPERTIES OUTPUT_NAME rio-server)
target_include_directories(rikerio-bin PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rikerio-bin systemd rt)
install (TARGETS rikerio-bin RUNTIME DESTINATION sbin)

add_library(rikerio-lib SHARED ${SRC}/rikerio.c)
set_target_properties(rikerio-lib PROPERTIES OUTPUT_NAME rikerio)
target_include_directories(rikerio-lib PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rikerio-lib rt)
install (TARGETS rikerio-lib LIBRARY DESTINATION lib)

add_executable(rikerio-test ${SRC}/test.c)
set_target_properties(rikerio-test PROPERTIES OUTPUT_NAME test)
target_include_directories(rikerio-test PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rikerio-test systemd rt rikerio-lib)

add_executable(rio-alias-add ${SRC}/add-alias.c)
set_target_properties(rio-alias-add PROPERTIES OUTPUT_NAME rio-alias-add)
target_include_directories(rio-alias-add PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rio-alias-add rt rikerio-lib)

add_executable(rio-alias-rm ${SRC}/rm-alias.c)
set_target_properties(rio-alias-rm PROPERTIES OUTPUT_NAME rio-alias-rm)
target_include_directories(rio-alias-rm PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rio-alias-rm rt rikerio-lib)

add_executable(rio-alias-ls ${SRC}/list-alias.c)
set_target_properties(rio-alias-ls PROPERTIES OUTPUT_NAME rio-alias-ls)
target_include_directories(rio-alias-ls PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rio-alias-ls rt rikerio-lib)



