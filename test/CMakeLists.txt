enable_testing()

include(googletest)
include_directories(${GTEST_INC_DIR} ${PROJECT_SOURCE_DIR}/test)

file(GLOB LIBRARY_COMMON_FILES ${SRC}/common/*.cpp)
file(GLOB LIBRARY_CLIENT_FILES ${SRC}/client/*.cpp)
file(GLOB LIBRARY_TEST_FILES ${TST}/lib/*.test.cpp)

message(STATUS "Library Common Files" ${LIBRARY_COMMON_FILES})
message(STATUS "Library Client Files" ${LIBRARY_CLIENT_FILES})
message(STATUS "Library Test Files" ${LIBRARY_TEST_FILES})

add_executable(test-lib 
    ${LIBRARY_COMMON_FILES} 
    ${LIBRARY_CLIENT_FILES} 
    ${LIBRARY_TEST_FILES})

target_include_directories(test-lib PUBLIC 
    ${PROJECT_SOURCE_DIR}/include 
    ${PROJECT_BINARY_DIR}/include)

target_link_libraries(test-lib pthread ${GTEST_LIB})
add_dependencies(test-lib ${GTEST_PREFIX})
