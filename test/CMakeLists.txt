cmake_minimum_required(VERSION 3.21)

project(stl_test)

link_directories(${CMAKE_SOURCE_DIR}/lib)

set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)
set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_ROOT_DIR}/queue)

add_executable(stl_test 
    ${PROJECT_ROOT_DIR}/test/queue/test_priority_queue.cpp)

target_link_libraries(stl_test gtest gtest_main pthread)
