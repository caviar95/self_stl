cmake_minimum_required(VERSION 3.10)
project(async_logger)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# file(GLOB SOURCES "src/*.cpp")

# add_library(logger STATIC ${SOURCES})

add_executable(test_logger test/test_logger.cpp)

target_link_libraries(test_logger pthread)
