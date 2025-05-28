

## 🧱 总体架构概览（职责分工）

| 模块文件                        | 主要职责说明                                     |
| --------------------------- | ------------------------------------------ |
| `main.cpp`                  | 应用入口，构建核心组件（`EventLoop`、`Acceptor`），启动事件循环 |
| `EventLoop.{h,cpp}`         | 核心调度模块（Reactor 主体），封装 `epoll`，负责注册/监听/分发事件 |
| `Channel.{h,cpp}`           | IO 通道抽象，表示一个 fd 和其关注的事件及其回调处理函数            |
| `Acceptor.{h,cpp}`          | 专用于监听新连接请求，封装 `listen socket` 和其回调         |
| `ConnectionHandler.{h,cpp}` | 表示一个具体客户端连接，封装对其 `fd` 的事件监听和处理逻辑           |
| `CMakeLists.txt`            | 构建配置文件                                     |
| `build/`                    | 编译产物目录                                     |

---

## 🔧 模块详解

---

### 1️⃣ `main.cpp` – 入口主函数

```cpp
int main() {
    EventLoop loop;

    Acceptor acceptor(&loop, 12345, [&](int conn_fd) {
        new ConnectionHandler(&loop, conn_fd);
    });

    loop.loop();
    return 0;
}
```

#### 主要功能：

* 创建 `EventLoop` 对象（Reactor）；
* 创建 `Acceptor` 对象监听 `12345` 端口；
* 注册新连接到来的回调，创建 `ConnectionHandler` 来管理每个客户端连接；
* 启动事件主循环 `loop()`，不断监听并分发事件。

---

### 2️⃣ `EventLoop.{h,cpp}` – 核心事件循环（Reactor）

#### 成员：

```cpp
int epoll_fd_;                      // epoll 实例
void updateChannel(Channel*);       // 注册一个通道到 epoll
void loop();                        // 事件主循环
```

#### 功能说明：

| 函数                        | 功能                                           |
| ------------------------- | -------------------------------------------- |
| `EventLoop()`             | 创建 `epoll` 实例                                |
| `updateChannel(Channel*)` | 把通道 `Channel` 注册到 `epoll` 中（注册监听的 fd 和其关注事件） |
| `loop()`                  | 无限循环中调用 `epoll_wait` 检测事件，一旦有事件发生，调用该通道绑定的回调 |

---

### 3️⃣ `Channel.{h,cpp}` – IO 通道抽象（封装 fd + 事件 + 回调）

#### 成员：

```cpp
int fd_;                   // 对应的文件描述符
int events_;               // 监听的事件（如 EPOLLIN）
Callback read_callback_;   // 可读事件的回调
```

#### 功能：

| 函数                    | 功能              |
| --------------------- | --------------- |
| `setReadCallback(cb)` | 设置可读事件时调用的回调函数  |
| `handleEvent()`       | 触发回调，处理事件       |
| `fd()` / `events()`   | 访问器，用于 epoll 注册 |

#### 特点：

* 这是典型的“观察者”模式，一个 `Channel` 表示一个 IO 事件源（fd），并且持有处理该事件的回调函数。

---

### 4️⃣ `Acceptor.{h,cpp}` – 监听器，处理新连接接入

```cpp
int listen_fd_;            // 服务端监听 socket
Channel channel_;          // 监听 socket 的事件封装
Callback new_conn_cb_;     // 新连接的回调（由 main 中传入）
```

#### 功能流程：

1. 创建监听套接字 `socket()`，绑定端口，设置为非阻塞；
2. 封装为 `Channel`，注册 `EPOLLIN`（新连接到来）事件；
3. 设置读回调 `read_callback_`，调用 `accept()` 接收连接；
4. 一旦接收到连接，调用 `new_conn_cb_`，由 `main` 创建 `ConnectionHandler`。

---

### 5️⃣ `ConnectionHandler.{h,cpp}` – 客户端连接处理器

```cpp
int conn_fd_;         // 客户端连接 socket
Channel channel_;     // 该连接的通道，监听其 IO 事件
```

#### 功能：

1. 注册客户端连接对应 fd 的 `EPOLLIN` 事件；
2. 设置回调函数（读事件处理）；
3. 在回调中执行：读取数据 + 回显数据（echo server）；
4. 如果连接关闭（n == 0），关闭 socket。

```cpp
ssize_t n = read(conn_fd_, buf, sizeof(buf));
if (n > 0) {
    write(conn_fd_, buf, n);  // 回显客户端数据
} else if (n == 0) {
    close(conn_fd_);
}
```

---

### 6️⃣ CMakeLists.txt – 构建系统

```cmake
add_executable(ReactorDemo
    main.cpp
    EventLoop.cpp
    Channel.cpp
    Acceptor.cpp
    ConnectionHandler.cpp
)
```

将所有模块编译为可执行文件 `ReactorDemo`。

---

## 🔁 核心事件循环回顾

事件分发流程如下：

```text
1. Acceptor 注册监听 socket 的读事件（EPOLLIN）；
2. 主线程 epoll_wait 检测到连接事件；
3. Acceptor::read_callback 被触发，accept 获取新 fd；
4. 将新 fd 封装为 ConnectionHandler + 注册读事件；
5. 有数据读事件时，调用 ConnectionHandler::read_callback；
6. 完成业务处理（如 echo、日志记录、关闭等）；
```

---

## 📌 结构优点

* 各模块职责清晰：主循环、接收器、连接处理器完全解耦；
* 所有 IO 非阻塞；
* 结构可拓展为多线程/多 Reactor 架构；
* 非常贴近 Nginx、Muduo 等真实项目的基础结构。

---



* 添加 **日志系统 / 状态监控**；
* 支持 **多线程主从 Reactor**；
* 添加 **写事件监听 / 粘包处理 / 协议层封装**；

