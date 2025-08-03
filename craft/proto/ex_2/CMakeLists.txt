cmake_minimum_required(VERSION 3.15)
project(StatusNameStubExample)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)

# 查找 protobuf
find_package(Protobuf REQUIRED)

# 生成 .pb.h/.pb.cc（只用于头文件，不编译 .pb.cc）
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS status.proto)

# 添加可执行文件，不链接 .pb.cc，只链接你的 stub
add_executable(main
  ${PROTO_HDRS}
  status_name_stub.cc
  main.cc
)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(main PRIVATE ${Protobuf_LIBRARIES})
