# cmake_minimum_required(VERSION 3.10)
# project(ProcessLauncher)

# set(CMAKE_CXX_STANDARD 17)

# # JSON 库（https://github.com/nlohmann/json）
# include(FetchContent)
# FetchContent_Declare(
#     json
#     URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
# )
# FetchContent_MakeAvailable(json)

# include_directories(include)

# add_executable(process_launcher
#     src/main.cpp
#     src/process_manager.cpp
#     src/registry_manager.cpp
# )

# target_link_libraries(process_launcher PRIVATE nlohmann_json::nlohmann_json)



cmake_minimum_required(VERSION 3.10)
project(ProcessLauncher)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    include
    third_party/json
)

add_executable(process_launcher
    src/main.cpp
    src/process_manager.cpp
    src/registry_manager.cpp
)
