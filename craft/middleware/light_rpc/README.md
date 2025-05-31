# 代码框架



rpc_server.h / cpp
使用 std::thread + select 管理 TCP 连接。

注册服务，接收请求，派发处理，返回响应。

rpc_client.h / cpp
连接 RPC 服务端，支持同步或异步发起调用。

rpc_channel.h / cpp
定义 RPC 通信的通道抽象，管理请求与响应的序列化和传输。

rpc_codec.h / cpp
负责消息封装/拆包（格式：消息长度 + 消息体，默认用 JSON 或二进制协议）。

service_registry.h / cpp
支持服务名与函数指针的注册与查找。

好的，我们将从零开始构建一个轻量级 C++ RPC 框架，具备以下能力：

---

## 框架目标功能

| 功能模块               | 描述                                                |
| ------------------ | ------------------------------------------------- |
| **网络通信层**          | 完全基于 TCP，自定义协议封包与拆包机制                             |
| **Protobuf 支持**    | 使用 Google Protobuf 作为接口定义和序列化方式                   |
| **同步调用**           | 支持客户端调用同步接口并等待返回结果                                |
| **异步回调**           | 支持客户端传入 `std::function<void(std::string)>` 异步回调方式 |
| **future/promise** | 支持客户端获取 `std::future<T>` 来异步处理调用结果                |
| **服务端注册**          | 服务端可注册多个服务与方法，自动映射 Protobuf 消息类型                  |
| **错误处理机制**         | 支持异常回传与失败处理                                       |

---
 
## 工程结构

```bash
light_rpc/
├── CMakeLists.txt
├── proto/
│   └── echo.proto
├── src/
│   ├── rpc_server.hpp / cpp        # 服务端核心逻辑
│   ├── rpc_client.hpp / cpp        # 客户端核心逻辑
│   ├── rpc_channel.hpp / cpp       # RPC 通道（封装连接+调用）
│   ├── rpc_controller.hpp / cpp    # 控制器（用于异常/状态传递）
│   ├── net/
│   │   ├── tcp_server.hpp / cpp
│   │   ├── tcp_client.hpp / cpp
│   │   └── socket_utils.hpp / cpp
│   ├── codec/
│   │   ├── message.hpp / cpp       # 自定义消息结构体
│   │   ├── encoder.hpp / cpp
│   │   └── decoder.hpp / cpp
│   ├── utils/
│   │   ├── thread_pool.hpp
│   │   └── log.hpp
├── examples/
│   ├── client_main.cpp
│   └── server_main.cpp
└── generated/
    ├── echo.pb.h
    └── echo.pb.cc
```

---

### 🔧 第一步：通信协议设计（简化版）

* 每个请求/响应包头结构：

```cpp
struct RpcMessageHeader {
    uint32_t body_size;       // Protobuf body size
    uint32_t method_name_len; // Method name length
};
```

* 总体格式：

```
[Header(8字节)] + [MethodName] + [Protobuf序列化Body]
```

---

### ✅ 接下来计划（阶段分布）：

| 阶段 | 模块           | 说明                                       |
| -- | ------------ | ---------------------------------------- |
| 1  | 工程初始化        | 创建完整 CMake 项目，整合 protobuf 编译流程           |
| 2  | 通信模块         | 实现 tcp\_server、tcp\_client、socket\_utils |
| 3  | 自定义协议封装      | 定义 header/encoder/decoder/message        |
| 4  | RPC 通道       | 客户端发送请求，服务端收到解析方法名+参数                    |
| 5  | 注册与反射        | 自动注册服务方法，Protobuf Message 映射             |
| 6  | 支持回调与 future | 添加异步机制支持：回调+promise/future               |
| 7  | 示例           | echo.proto 的 client/server 示例            |
| 8  | 进阶扩展         | 添加压缩、超时、链路追踪、线程池等能力                      |


# 轻量 RPC 框架的通信协议

包括消息结构、序列化方案、请求/响应封装规则等。这是整个 RPC 框架的“语言”。

---

## 🧱 协议设计目标

| 项目     | 目标                                 |
| ------ | ---------------------------------- |
| 封包结构   | 解决 TCP 流的粘包与拆包问题，便于服务端解析完整消息       |
| 方法标识   | 支持 RPC 方法名映射（如 `EchoService.Echo`) |
| 参数序列化  | 使用 Protobuf 编码 RPC 请求参数和响应值        |
| 异常与状态码 | 支持回传错误状态/异常信息                      |
| 可扩展性   | 协议应预留扩展字段（如 trace\_id、压缩标识等）       |

---

## 📦 协议结构草图

我们采用如下封包格式：

```
[Total Length (4B)]
[Header Length (4B)]
[Header (JSON or Proto)]
[Body (Protobuf serialized message)]
```

---

### 1️⃣ 消息总览

| 字段             | 类型        | 说明                    |
| -------------- | --------- | --------------------- |
| total\_length  | uint32\_t | 整个包的长度（含自身4字节）        |
| header\_length | uint32\_t | header区的长度（字节数）       |
| header         | bytes     | 可用 JSON 或自定义 proto 定义 |
| body           | bytes     | Protobuf 序列化的数据部分     |

---

### 2️⃣ Header 结构定义（使用 JSON/Proto 均可）

示例（使用 JSON 展示）：

```json
{
  "msg_id": 12345,                        // 唯一请求ID，用于异步匹配
  "service_name": "EchoService",         // 服务名
  "method_name": "Echo",                 // 方法名
  "is_request": true,                    // 请求 or 响应
  "error": "",                           // 错误信息（响应才填）
  "meta": {
    "compress": "none",                  // 压缩算法（可选）
    "trace_id": "abc-123-xyz"            // 链路追踪ID（可选）
  }
}
```

可以选择：

* 初期用 **JSON 作为 header**，调试方便；
* 后期可优化为 Protobuf。

---

### 3️⃣ body：Protobuf 数据

* request 的 body：就是 Protobuf 定义的 Request message
* response 的 body：是 Response message
* 支持用户自由定义

---

## 📚 举例：一次 RPC 请求数据包结构如下

设有 Protobuf 服务：

```proto
service EchoService {
  rpc Echo(EchoRequest) returns (EchoResponse);
}
```

一次调用的数据流为：

```
发送方（客户端）：
[total_len]
[header_len]
{ "service_name": "EchoService", "method_name": "Echo", ... }
[EchoRequest protobuf encoded]

接收方（服务端）：
解析 header → 反射方法映射 → 解析 body → 调用方法 → 返回响应
```

---

## 🛠 拆包策略

每个连接上维护状态机：

1. 读取 total\_len（4字节）
2. 读取 header\_len（4字节）
3. 读取 header
4. 读取 body
5. 组装成 RpcMessage 结构体，投递执行

---

## ✅ RpcMessage（最终 C++ 封装结构）

```cpp
struct RpcMessage {
    uint32_t msg_id;
    std::string service_name;
    std::string method_name;
    bool is_request;
    std::string error;

    std::string meta_json;  // trace_id 等
    std::string body;       // raw Protobuf buffer
};
```

---

接下来我们可以进入：

### 👉 下一步：定义 C++ 的 `RpcMessage` + `Encoder/Decoder` 模块，支持组包/拆包

是否现在开始进入此阶段？我可以立即生成 `src/codec/message.hpp/cpp + encoder.hpp/cpp + decoder.hpp/cpp`。
