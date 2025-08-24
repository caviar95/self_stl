cmake_minimum_required(VERSION 3.28)

project(proto_gtest_stub)

set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS} include)

add_subdirectory(src)

add_subdirectory(tests)

