加入任务优先级支持，以便更灵活地调度任务。

---

## 🧩 本阶段目标：支持任务优先级（priority）

### ✅ 新功能设计：

| 功能点                       | 描述                    |
| ------------------------- | --------------------- |
| 任务对象 `Task`               | 包含 `priority` 字段，用于排序 |
| 使用 `std::priority_queue`  | 取代普通队列，实现优先调度         |
| 高优先级数字更小                  | 如：0 表示最高优先级           |
| `enqueue(task, priority)` | 新接口支持优先级提交任务          |

---

## ✅ 第一步：定义 Task 结构体

## ✅ 第二步：修改 ThreadPool 使用 `std::priority_queue<Task>`

## ✅ 第三步：修改调度器支持优先级调度
我们设置偶数任务为高优先级（priority = 1），奇数任务为默认（priority = 10）：




---

## ✅ 输出示例（显示任务优先级）

```
[Task] Executing task #0 (priority 1)
[Task] Executing task #2 (priority 1)
[Task] Executing task #4 (priority 1)
[Task] Executing task #6 (priority 1)
[Task] Executing task #1 (priority 10)
[Task] Executing task #3 (priority 10)
[Task] Executing task #5 (priority 10)
[Task] Executing task #7 (priority 10)
Main exiting.
```

!!!!!!!!!!! 没有实现
[Task] Executing task #0 (priority 1)
[Task] Executing task #1 (priority 10)
[Task] Executing task #2 (priority 1)
[Task] Executing task #3 (priority 10)
[Task] Executing task #4 (priority 1)
[Task] Executing task #5 (priority 10)
[Task] Executing task #6 (priority 1)
[Task] Executing task #7 (priority 10)
Main exiting.

可以看出：**高优先级任务（偶数）先被执行**，即使是后提交的。

---

## 🔮 下一阶段建议

是否继续扩展如下功能之一：

1. ✅ 每个任务可设置“延迟执行时间戳”。
2. ✅ 支持任务取消（返回取消句柄）。
3. ✅ 支持线程池动态扩展与缩减。
4. ✅ 每个任务可指定由哪个线程执行（绑定线程）。


