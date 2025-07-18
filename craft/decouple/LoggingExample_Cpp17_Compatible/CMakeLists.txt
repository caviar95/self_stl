cmake_minimum_required(VERSION 3.14)
project(LoggingExampleCpp17 CXX)

set(CMAKE_CXX_STANDARD 17)

# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/refs/tags/release-1.14.0.zip
# )
# FetchContent_MakeAvailable(googletest)

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