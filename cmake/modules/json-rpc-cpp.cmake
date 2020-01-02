set(JSONRPC_PREFIX       libjson-rpc-cpp)
set(JSONRPC_BUILD_PREFIX libjson-rpc-cpp-build) 
set(JSONRPC_VERSION      v1.2.0)
set(JSONRPC_GIT_URL      https://github.com/cinemast/libjson-rpc-cpp)
set(JSONRPC_BASE_DIR     ${PROJECT_BINARY_DIR}/${JSONRPC_PREFIX}-${JSONRPC_VERSION})
set(JSONRPC_SRC_DIR      ${JSONRPC_BASE_DIR})
set(JSONRPC_BIN_DIR      ${JSONRPC_BASE_DIR}-build)
set(JSONRPC_INSTALL_DIR  ${CMAKE_BINARY_DIR}/install)
set(JSONRPC_INC          ${JSONRPC_INSTALL_DIR}/include/)
set(JSONRPC_LIB_COMMON   ${JSONRPC_INSTALL_DIR}/lib/libjsonrpccpp-common.a)
set(JSONRPC_LIB_SERVER   ${JSONRPC_INSTALL_DIR}/lib/libjsonrpccpp-server.a)
set(JSONRPC_LIB_CLIENT   ${JSONRPC_INSTALL_DIR}/lib/libjsonrpccpp-client.a)


ExternalProject_Add(${JSONRPC_PREFIX}
    PREFIX ${JSONRPC_PREFIX}
    GIT_REPOSITORY ${JSONRPC_GIT_URL}
    GIT_TAG ${JSONRPC_VERSION}
    BINARY_DIR ${JSONRPC_BIN_DIR}
    CMAKE_ARGS
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/install
      -DCMAKE_PREFIX_PATH=${PROJECT_BINARY_DIR}/install
      -DHTTP_SERVER=NO
      -DHTTP_CLIENT=NO
      -DCOMPILE_TESTS=NO
      -DREDIS_SERVER=NO
      -DREDIS_CLIENT=NO
      -DCOMPILE_EXAMPLES=NO
      -DCOMPILE_STUBGEN=NO
      -DWITH_COVERAGE=NO
      -DBUILD_SHARED_LIBS=NO
      -DBUILD_STATIC_LIBS=YES
      -DUNIX_DOMAIN_SOCKET_SERVER=YES
      -DUNIX_DOMAIN_SOCKET_CLIENT=YES
    LOG_DOWNLOAD 1
    LOG_BUILD 1
    STEP_TARGETS build
    DEPENDS libjsoncpp
    EXCLUDE_FROM_ALL TRUE)


message(STATUS "lib jsonrpccpp common ... ${JSONRPC_LIB_COMMON}")
message(STATUS "lib jsonrpccpp server ... ${JSONRPC_LIB_SERVER}")
message(STATUS "lib jsonrpccpp client ... ${JSONRPC_LIB_CLIENT}")