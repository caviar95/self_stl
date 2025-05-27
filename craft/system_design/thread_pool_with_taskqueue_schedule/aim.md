


## 系统目标架构：线程池 + 任务队列 + 条件变量 + 时间调度

### 1. **线程池（ThreadPool）**

* 固定数量的工作线程
* 每个线程等待任务队列中的新任务

### 2. **任务队列（TaskQueue）**

* 支持线程安全的任务入队/出队
* 使用 `std::queue<std::function<void()>>`

### 3. **条件变量**

* 用于唤醒空闲线程去执行新任务

### 4. **调度器（Scheduler）**

* 定时投递任务到线程池，比如每隔 1 秒

---

## 🧩 设计模块分工

| 模块           | 功能描述                 |
| ------------ | -------------------- |
| `ThreadPool` | 管理线程、任务队列、同步机制       |
| `TaskQueue`  | 封装互斥访问的任务容器          |
| `Scheduler`  | 每秒投递一个任务（模拟定时器）      |
| `main`       | 启动线程池、启动定时调度器、运行示例任务 |

---



## ✅ 第一步：线程池代码（基本实现）
## 🕒 第二步：定时调度器模块
## 🧪 第三步：主程序使用

## ✅ 输出示例：

```
[Task] Executing scheduled task #1
[Task] Executing scheduled task #2
[Task] Executing scheduled task #3
[Task] Executing scheduled task #4
[Task] Executing scheduled task #5
[Task] Executing scheduled task #6
Main exiting.
```
!!!!! 该目标未达成
[Task] Executing scheduled task #1
[Task] Executing scheduled task #1
[Task] Executing scheduled task #1
[Task] Executing scheduled task #1
[Task] Executing scheduled task #1
[Task] Executing scheduled task #1
Main exiting.

---

## 🔄 下一步可扩展方向

你可以进一步拓展：

1. ✅ 任务支持定时投递时间（delay 时间戳）
2. ✅ 任务支持取消（返回一个可取消的句柄）
3. ✅ 任务优先级队列（使用 `std::priority_queue`）
4. ✅ 动态线程扩展和回收
5. ✅ 每个任务绑定到某个指定线程（绑定线程 ID）

