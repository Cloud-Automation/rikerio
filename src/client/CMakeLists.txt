set(CLIENT_SRC_FILES ${SRC}/client/client.cpp
    ${SRC}/client/request/memory-alloc.cpp
    ${SRC}/client/request/memory-dealloc.cpp
    ${SRC}/client/request/data-add.cpp
    ${SRC}/client/request/data-remove.cpp
    ${SRC}/client/request/data-list.cpp
    ${SRC}/client/request/link-add.cpp
    ${SRC}/client/request/link-remove.cpp
    ${SRC}/client/request/link-list.cpp
    ${SRC}/client/response/config-get.cpp
    ${SRC}/client/response/memory-alloc.cpp
    ${SRC}/client/response/memory-dealloc.cpp
    ${SRC}/client/response/memory-list.cpp
    ${SRC}/client/response/data-add.cpp
    ${SRC}/client/response/data-remove.cpp
    ${SRC}/client/response/data-list.cpp
    ${SRC}/client/response/link-add.cpp
    ${SRC}/client/response/link-remove.cpp
    ${SRC}/client/response/link-list.cpp)


add_library(rikerio ${COMMON_SRC_FILES} ${CLIENT_SRC_FILES})
target_link_libraries(rikerio ${JSONRPC_LIB_CLIENT} ${JSONRPC_LIB_COMMON} ${JSONCPP_LIB})
add_dependencies(rikerio common)

install (TARGETS rikerio LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install (DIRECTORY ${PROJECT_BINARY_DIR}/include DESTINATION lib)

