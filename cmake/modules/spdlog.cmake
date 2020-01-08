set(SPDLOG_PREFIX       spdlog)
set(SPDLOG_VERSION      v1.4.2)
set(SPDLOG_GIT_URL      https://github.com/gabime/spdlog)
set(SPDLOG_BINARY_DIR   ${PROJECT_BINARY_DIR}/${SPDLOG_PREFIX}-${SPDLOG_VERSION})
set(SPDLOG_SOURCE_DIR   ${PROJECT_BINARY_DIR}/${SPDLOG_PREFIX}/src/${SPDLOG_PREFIX})
set(SPDLOG_LIB          ${SPDLOG_BINARY_DIR}/libspdlog.a)

ExternalProject_Add(${SPDLOG_PREFIX}
    PREFIX ${SPDLOG_PREFIX}
    GIT_REPOSITORY ${SPDLOG_GIT_URL}
    GIT_TAG ${SPDLOG_VERSION}
    BINARY_DIR ${SPDLOG_BINARY_DIR}
    CMAKE_ARGS
        -DSPDLOG_MASTER_PROJECT=OFF
    STEP_TARGETS build
    EXCLUDE_FROM_ALL TRUE)

include_directories(${SPDLOG_SOURCE_DIR}/include)

