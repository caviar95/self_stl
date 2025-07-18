cmake_minimum_required(VERSION 3.14)
project(LoggingExample CXX)

set(CMAKE_CXX_STANDARD 14)

include_directories(
  platform
  logging
  user

  ../../../test/lib
)

link_directories(../../../test/lib)

enable_testing()

include_directories(
  platform
  logging
  user
)

add_library(logging_example
  user/user_module.cpp
)

add_executable(tests
  tests/test_main.cpp
  tests/test_user_module.cpp
)

target_link_libraries(tests
  logging_example
  gtest
  gmock
  gtest_main
)
