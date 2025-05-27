#pragma once

#include <memory>

class execution_context {};


class thread_pool : public execution_context {
public:
    // 前向声明模板类  
    // 告诉编译器`basic_executor_type`是一个嵌套的类模板，但此时尚未定义。
    template <typename Allocator, unsigned int Bits>
    class basic_executor_type;

    // 声明模板友元
    // 将`basic_executor_type`的所有模板特例声明为`thread_pool`的友元。使得任何`basic_executor_type<Allocator, Bits>`的实例都能访问`thread_pool`的私有成员
    // `basic_executor_type`作为执行器，需要与`thread_pool`内部交互（如提交任务、管理线程），而这些操作可能涉及线程池的私有数据（如任务队列）。通过友元声明，执行器可以直接访问这些私有部分，而无需暴露给外部
    // 两次声明解决了模板类作为友元的依赖问题，确保了执行器与线程池的紧密集成，同时维护了代码的封装性。
    template <typename Allocator, unsigned int Bits>
    friend class basic_executor_type;

    using executor_type = basic_executor_type<std::allocator<void>, 0>;

    thread_pool();

    thread_pool(std::size_t num_threads);

    thread_pool(std::size_t num_threads, const execution_context::service_maker &initial_services);

    ~thread_pool();

    executor_type get_executor() noexcept;

    executor_type executor() noexcept;

    inline void stop();

    inline void attach();

    inline void join();

    inline void wait();

private:
    thread_pool(const thread_pool&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;

    struct thread_function;

    inline detail::scheduler& add_sceduler(detail::scheduler* s);

    detail::scheduler &scheduler_;

    detail::thread_group threads_;

    detail::atomic_count num_threads_;

    bool joinable_;
};
