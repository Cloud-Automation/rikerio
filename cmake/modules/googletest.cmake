set(GTEST_PREFIX      gtest)
#set(GTEST_VERSION     master)
set(GTEST_BIN_DIR     ${PROJECT_BINARY_DIR}/${GTEST_PREFIX}-build)
set(GTEST_INSTALL_DIR ${PROJECT_BINARY_DIR}/${GTEST_PREFIX}-install)
set(GTEST_SRC_DIR     ${GTEST_INSTALL_DIR})
set(GTEST_INC_DIR     ${GTEST_INSTALL_DIR}/include)
set(GTEST_LIB_DIR     ${GTEST_INSTALL_DIR}/lib)
set(GTEST_LIB         ${GTEST_LIB_DIR}/libgtest_main.a 
                        ${GTEST_LIB_DIR}/libgtest.a 
                        ${GTEST_LIB_DIR}/libgmock.a 
                        ${GTEST_LIB_DIR}/libgmock_main.a)

ExternalProject_Add(${GTEST_PREFIX}
    PREFIX ${GTEST_PREFIX}
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG ${GTEST_VERSION}
    BINARY_DIR ${GTEST_BIN_DIR}
    CMAKE_ARGS 
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL_DIR} 
    LOG_DOWNLOAD 1
    LOG_BUILD 1
    UPDATE_DISCONNECTED 1
    STEP_TARGETS build
    EXCLUDE_FROM_ALL TRUE)

message(STATUS "${GTEST_PREFIX} ... Library directory ${GTEST_LIB_DIR}")
message(STATUS "${GTEST_PREFIX} ... Include directory ${GTEST_INC_DIR}")

