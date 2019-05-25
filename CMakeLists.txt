cmake_minimum_required (VERSION 3.2.0)
project (RikerIO C)

option(BUILD_STATIC "Compile as static library."    OFF)
option(WITH_TESTS   "Build with tests"              OFF)
option(TARGET_ARCH  "Build for target"              "x86")

if (${WITH_SYSTEMD})
	add_definitions(-DWITH_SYSTEMD)
endif()

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
    enable_testing()
endif()


include(ExternalProject)
include(GetGitRevisionDescription)
include(version)
include(cpack)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/version.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/rikerio.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/rikerio.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/install.sh ${CMAKE_CURRENT_BINARY_DIR}/postinst)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/rio.in ${CMAKE_CURRENT_BINARY_DIR}/rio COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/rikerio@.service ${CMAKE_CURRENT_BINARY_DIR}/rikerio@.service COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/rio-completion.bash ${CMAKE_CURRENT_BINARY_DIR}/rio-completion.bash COPYONLY)


file(GLOB HEADERS ${INC}/*.h)
file(GLOB SOURCES ${SRC}/*.c)

add_executable(rio-profile-create ${SRC}/server.c)
target_include_directories(rio-profile-create PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rio-profile-create rt dl)

add_library(rikerio-lib SHARED ${SRC}/rikerio.c)
set_target_properties(rikerio-lib PROPERTIES OUTPUT_NAME rikerio)
set_property(TARGET rikerio-lib PROPERTY C_STANDARD 99)
set_property(TARGET rikerio-lib PROPERTY PUBLIC_HEADER ${CMAKE_CURRENT_BINARY_DIR}/include/rikerio.h)
target_include_directories(rikerio-lib PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rikerio-lib rt)

add_executable(rikerio-test ${SRC}/test.c)
set_target_properties(rikerio-test PROPERTIES OUTPUT_NAME test)
set_property(TARGET rikerio-test PROPERTY C_STANDARD 99)
target_include_directories(rikerio-test PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rikerio-test rt rikerio-lib)

add_executable(rio-alias-add ${SRC}/add-alias.c)
set_target_properties(rio-alias-add PROPERTIES OUTPUT_NAME rio-alias-add)
set_property(TARGET rio-alias-add PROPERTY C_STANDARD 99)
target_include_directories(rio-alias-add PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rio-alias-add rt rikerio-lib)

add_executable(rio-alias-rm ${SRC}/rm-alias.c)
set_target_properties(rio-alias-rm PROPERTIES OUTPUT_NAME rio-alias-rm)
set_property(TARGET rio-alias-rm PROPERTY C_STANDARD 99)
target_include_directories(rio-alias-rm PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rio-alias-rm rt rikerio-lib)

add_executable(rio-alias-inspect ${SRC}/list-alias.c)
set_target_properties(rio-alias-inspect PROPERTIES OUTPUT_NAME rio-alias-inspect)
set_property(TARGET rio-alias-inspect PROPERTY C_STANDARD 99)
target_include_directories(rio-alias-inspect PUBLIC ${INC} ${PROJECT_BINARY_DIR})
target_link_libraries(rio-alias-inspect rt rikerio-lib)



install (TARGETS rikerio-lib rio-profile-create rio-alias-add rio-alias-rm rio-alias-inspect
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
    RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rio DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rikerio@.service DESTINATION /lib/systemd/system)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rio-completion.bash DESTINATION /etc/bash_completion.d)
#install(CODE "execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/install.sh ${CMAKE_INSTALL_PREFIX})")

