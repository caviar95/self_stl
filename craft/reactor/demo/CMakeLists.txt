cmake_minimum_required(VERSION 3.10)
project(ReactorDemo)

set(CMAKE_CXX_STANDARD 17)

add_executable(ReactorDemo
    main.cpp
    event_loop.cpp
    channel.cpp
    acceptor.cpp
    connection_handler.cpp
)
