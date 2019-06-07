CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ADD_SUBDIRECTORY(googletest)

SET(CMAKE_CXX_STANDARD 11)

ADD_EXECUTABLE(rshell
    src/main.cpp
    src/connector.cpp
    src/parse.cpp
    src/execute.cpp
)

ADD_EXECUTABLE(test
    unit_tests/test.cpp
    src/connector.cpp
    src/parse.cpp
    src/execute.cpp
)

TARGET_LINK_LIBRARIES(test gtest)
TARGET_COMPILE_DEFINITIONS(test PRIVATE gtest_disable_pthreads=ON)

