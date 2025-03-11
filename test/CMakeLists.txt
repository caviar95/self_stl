cmake_minimum_required(VERSION 3.21)

project(stl_test)

link_directories(${CMAKE_SOURCE_DIR}/lib)

set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)
set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_ROOT_DIR}/queue)
include_directories(${PROJECT_ROOT_DIR}/thread)
include_directories(${PROJECT_ROOT_DIR}/data_struct)

set(THREAD_POOL_WITH_PRI_FILES 
    ${PROJECT_ROOT_DIR}/test/thread/test_thread.cpp
)

set(THREAD_POOL_WITH_SCHED_FILES
    ${PROJECT_ROOT_DIR}/test/thread/test_scheduler.cpp
)

add_executable(stl_test 
    ${PROJECT_ROOT_DIR}/test/queue/test_priority_queue.cpp
    ${PROJECT_ROOT_DIR}/test/data_struct/test_rbtree.cpp
    ${THREAD_POOL_WITH_SCHED_FILES}
)

target_link_libraries(stl_test gtest gtest_main pthread)
