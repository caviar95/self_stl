cmake_minimum_required(VERSION 3.10)
project(FinalLogger LANGUAGES CXX)

# 使用 C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 可执行文件
add_executable(FinalLogger
    main.cpp
    src/ProductLoggerImpl.cpp
)

# 对应平台源码在根目录下，无需额外链接
