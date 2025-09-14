cmake_minimum_required(VERSION 3.14)
project(MaskStateMachineDemo)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(main src/main.cpp)

# GoogleTest
enable_testing()
find_package(GTest REQUIRED)
add_executable(test_statemachine tests/test_state_machine.cpp)
target_link_libraries(test_statemachine GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_statemachine)
