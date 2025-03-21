cmake_minimum_required(VERSION 3.21)

project(stl_test)

link_directories(${CMAKE_SOURCE_DIR}/lib)

set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)
set(CMAKE_CXX_STANDARD 20)

add_definitions(-DOBJECT_POOL_MIN_MAX_SIZE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

include_directories(${PROJECT_ROOT_DIR}/queue)
include_directories(${PROJECT_ROOT_DIR}/thread)
include_directories(${PROJECT_ROOT_DIR}/data_struct)
include_directories(${PROJECT_ROOT_DIR}/pool)
include_directories(${PROJECT_ROOT_DIR}/utility)
include_directories(${PROJECT_ROOT_DIR}/design/mode/observer)
include_directories(${PROJECT_ROOT_DIR}/design/mode/states)

set(THREAD_POOL_WITH_PRI_FILES 
    ${PROJECT_ROOT_DIR}/test/thread/test_thread.cpp
)

set(THREAD_POOL_WITH_SCHED_FILES
    ${PROJECT_ROOT_DIR}/test/thread/test_scheduler.cpp
)

add_executable(stl_test 
    # ${PROJECT_ROOT_DIR}/test/queue/test_priority_queue.cpp
    # ${PROJECT_ROOT_DIR}/test/data_struct/test_rbtree.cpp
    # ${PROJECT_ROOT_DIR}/test/pool/test_object_pool.cpp
    # ${PROJECT_ROOT_DIR}/test/pool/test_particle_pool.cpp
    # ${PROJECT_ROOT_DIR}/test/pool/test_mem_pool.cpp
    # ${PROJECT_ROOT_DIR}/test/pool/test_object_alloc_pool.cpp
    # ${PROJECT_ROOT_DIR}/test/pool/test_mem_pool_base.cpp
    # ${PROJECT_ROOT_DIR}/test/pool/test_fixed_pool.cpp
    ${PROJECT_ROOT_DIR}/test/design/test_observer_mode.cpp
    ${PROJECT_ROOT_DIR}/test/design/test_states_mode.cpp
    # ${THREAD_POOL_WITH_SCHED_FILES}
)

target_link_libraries(stl_test gtest gtest_main pthread)
