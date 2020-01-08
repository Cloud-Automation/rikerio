
include(googletest)
include_directories(${GTEST_INC_DIR} ${PROJECT_SOURCE_DIR}/test)

file(GLOB CLIENT_TEST_FILES ${TST}/client/*.test.cpp)

add_executable(client-test ${CLIENT_TEST_FILES})
target_link_libraries(client-test ${GTEST_LIB} pthread rikerio)
add_dependencies(client-test ${GTEST_PREFIX}-build rikerio)
add_test(client.test ${PROJECT_BINARY_DIR}/test/client-test)
