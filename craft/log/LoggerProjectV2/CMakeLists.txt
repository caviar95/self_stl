cmake_minimum_required(VERSION 3.10)
project(LoggerProject)

set(CMAKE_CXX_STANDARD 17)
enable_testing()

# Logger Library
# add_library(logger
#     src/PlatformLogger.h
#     src/ILogger.h
#     src/BusinessLogger.h
# )

# Main Executable
add_executable(main src/main.cpp)
target_link_libraries(main 
# logger
)

# GoogleTest Setup
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
# )
# FetchContent_MakeAvailable(googletest)


include_directories(/home/river/Documents/code/self_stl/test/lib)
link_directories(/home/river/Documents/code/self_stl/test/lib)

# Test Executable
add_executable(logger_tests tests/test_logger.cpp)
target_link_libraries(logger_tests gtest gmock gtest_main gmock_main 
# logger
)

include(GoogleTest)
gtest_discover_tests(logger_tests)
