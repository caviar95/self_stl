目标：高性能异步日志系统

功能需求：
支持高并发、低延迟、线程安全、支持日志分级和异步落盘

模块功能拆分：

1 同步日志接口
基础封装、日志级别、格式化

2 异步日志存储
ring_buffer，log producer/consumer

3 多线程安全和性能优化
lock-free, CAS, spin-lock

4 日志过滤与分模块输出
日志级别控制，模块标签

5 时间戳、线程id、行号
std::chrono std::this_thread

6 持久化优化
批量flush写盘

7 日志采样、日志加密等

