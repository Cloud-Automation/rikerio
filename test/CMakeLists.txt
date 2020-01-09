
include(googletest)
include_directories(${GTEST_INC_DIR} ${PROJECT_SOURCE_DIR}/test)

file(GLOB CLIENT_TEST_FILES ${TST}/client/*.test.cpp)
file(GLOB COMMON_TEST_FILES ${TST}/common/*.test.cpp)

add_executable(client-test ${CLIENT_TEST_FILES})
target_link_libraries(client-test ${GTEST_LIB} pthread rikerio)
add_dependencies(client-test ${GTEST_PREFIX}-build rikerio)
add_test(client.test ${PROJECT_BINARY_DIR}/test/client-test)

add_executable(common-test ${COMMON_TEST_FILES})
target_link_libraries(common-test ${GTEST_LIB} pthread rikerio)
add_dependencies(common-test ${GTEST_PREFIX}-build rikerio)
add_test(common.test ${PROJECT_BINARY_DIR}/test/common-test)
