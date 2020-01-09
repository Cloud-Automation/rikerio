add_custom_target(common)

file(COPY ${INC}/rikerio.h DESTINATION ${PROJECT_BINARY_DIR}/include/rikerio)
file(COPY ${INC}/client DESTINATION ${PROJECT_BINARY_DIR}/include/rikerio)
file(COPY ${INC}/common DESTINATION ${PROJECT_BINARY_DIR}/include/rikerio)


set(COMMON_SRC_FILES 
    ${SRC}/common/type.cpp
    ${SRC}/common/mem-position.cpp
    ${SRC}/common/semaphore.cpp
    ${SRC}/common/config.cpp 
    PARENT_SCOPE)
