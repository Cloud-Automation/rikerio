set(SERVER_SRC_FILES 
    ${SRC}/server/server.cpp
    ${SRC}/server/memory.cpp
    ${SRC}/server/data.cpp
    ${SRC}/server/data-map.cpp
    ${SRC}/server/link-map.cpp)

set(SERVER_MAIN_FILE 
    ${SRC}/server/main.cpp)

add_executable(rikerio-server ${COMMON_SRC_FILES} ${SERVER_SRC_FILES} ${SERVER_MAIN_FILE})
target_include_directories(rikerio-server PUBLIC ${INC})
target_link_libraries(rikerio-server rt dl ${JSONRPC_LIB_SERVER} ${JSONRPC_LIB_COMMON} ${JSONCPP_LIB} pthread)
add_dependencies(rikerio-server common)

install (TARGETS rikerio-server RUNTIME DESTINATION bin)

