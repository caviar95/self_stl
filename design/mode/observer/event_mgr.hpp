/*
    实现原理
        单例类：保证整个程序中只有一个实例，方便管理观察者模式中的发布和注册操作。
        编译期或程序启动阶段完成操作：借助静态变量和构造函数，在程序启动时就完成观察者的注册，
            而不是在运行时动态注册。

    单例类 EventManager：
    getInstance 方法：使用静态局部变量确保单例实例的唯一性和线程安全。
    subscribe 方法：用于注册观察者，将观察者函数添加到 observers 向量中。
    publish 方法：遍历 observers 向量，调用每个观察者函数。
    禁用拷贝构造函数和赋值运算符，防止外部复制单例实例。
    观察者函数：observerFunction1 和 observerFunction2 是示例观察者函数，当事件发布时会被调用。
    静态初始化：
    Initializer 结构体：其构造函数在程序启动时被调用，在构造函数中完成观察者的注册。
    static Initializer initializer：确保 Initializer 结构体在程序启动时被构造，从而完成注册操作。

    注意事项
        静态初始化顺序问题：如果不同的单例类或静态对象之间存在依赖关系，需要注意静态初始化的顺序，避免出现未定义行为。
        异常处理：在实际应用中，需要考虑观察者函数可能抛出的异常，确保异常不会影响整个事件发布过程。
        线程安全：如果在多线程环境下使用，需要确保 subscribe 和 publish 方法的线程安全，可以使用互斥锁来保护共享资源。
 */

#pragma once

#include <functional>

class EventMgr {
public:
    static EventMgr& GetInstance() {
        static EventMgr instance;
        return instance;
    }

    void Subscribe(const std::function<void()>& observer) {
        observers_.push_back(observer);
    }

    void Publish() {
        for (const auto& observer : observers_) {
            observer();
        }
    }

    EventMgr(const EventMgr&) = delete;
    EventMgr& operator=(const EventMgr&) = delete;
    EventMgr(EventMgr&&) = delete;
    EventMgr& operator=(EventMgr&&) = delete;

    ~EventMgr() = default;

private:
    EventMgr() = default;

    std::vector<std::function<void()>> observers_;
};