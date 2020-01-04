set(CLI_SRC_FILES ${SRC}/cli/rio.cpp 
    ${SRC}/cli/config-get.cpp
    ${SRC}/cli/memory-alloc.cpp
    ${SRC}/cli/memory-dealloc.cpp
    ${SRC}/cli/memory-list.cpp
    ${SRC}/cli/data-add.cpp
    ${SRC}/cli/data-list.cpp
    ${SRC}/cli/data-remove.cpp
    ${SRC}/cli/link-add.cpp
    ${SRC}/cli/link-remove.cpp
    ${SRC}/cli/link-list.cpp)

add_executable(rio ${CLI_SRC_FILES})   
target_link_libraries(rio rikerio)
add_dependencies(rikerio-server common)

install (TARGETS rio RUNTIME DESTINATION bin)

