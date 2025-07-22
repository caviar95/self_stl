cmake_minimum_required(VERSION 3.10)
project(FinalLogger)

set(CMAKE_CXX_STANDARD 14)

include_directories(
./    
./include)

add_executable(main
    main.cpp
    src/ProductLoggerImpl.cpp
)
