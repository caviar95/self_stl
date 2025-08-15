cmake_minimum_required(VERSION 4.0)
project(MaskStateMachine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MaskStateMachine
    main.cpp
    # state_machine.cpp
)

# 可选：打开警告
if (MSVC)
  target_compile_options(MaskStateMachine PRIVATE /W4)
else()
  target_compile_options(MaskStateMachine PRIVATE -Wall -Wextra -Wpedantic)
endif()
