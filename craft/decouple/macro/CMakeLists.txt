cmake_minimum_required(VERSION 3.14)
project(LoggingExample CXX)

set(CMAKE_CXX_STANDARD 17)

# GoogleTest
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/refs/tags/release-1.14.0.zip
# )
# FetchContent_MakeAvailable(googletest)

enable_testing()


# Include directories
include_directories(
  platform
  logging
  user

  ../../../test/lib
)

link_directories(../../../test/lib)

# Main source
add_library(logging_example
  user/user_module.cpp
)

# Test binary
add_executable(tests
  test/test_main.cpp
  test/test_user_module.cpp
)

target_link_libraries(tests
  logging_example
  gtest
  gmock
#   gtest_main
)
