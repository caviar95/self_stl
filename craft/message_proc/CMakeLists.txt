cmake_minimum_required(VERSION 3.12)
project(MessageParserTest)

set(CMAKE_CXX_STANDARD 17)

find_package(yaml-cpp REQUIRED)

add_executable(parser_test 
    example.cpp
)

target_link_libraries(parser_test 
    PRIVATE yaml-cpp
)