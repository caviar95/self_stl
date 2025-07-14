cmake_minimum_required(VERSION 3.14)
project(auto_registry_demo)

set(CMAKE_CXX_STANDARD 17)

add_executable(auto_registry_demo
    main.cpp
    logger.hpp
    registry.hpp
)
