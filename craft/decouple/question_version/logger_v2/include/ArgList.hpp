#pragma once
#include <tuple>
#include <utility>
#include <functional>

// —— 类型擦除的参数列表基类 ——  
struct IArgList {
    virtual ~IArgList() = default;
    // 拆包并回调 f(arg...)
    template <typename F>
    void apply(F&& f) {
        applyImpl(reinterpret_cast<void*>(&f));
    }
private:
    virtual void applyImpl(void* fnPtr) = 0;
};

// —— 具体参数列表实现 ——  
template <typename... Args>
struct ArgList : IArgList {
    std::tuple<typename std::decay<Args>::type...> tpl;
    ArgList(Args&&... args)
      : tpl(std::forward<Args>(args)...) {}

private:
    void applyImpl(void* fnPtr) override {
        // 恢复 std::function<void(Args...)> 类型
        auto& f = *reinterpret_cast<std::function<void(Args...)>*>(fnPtr);
        applyTuple(f, std::make_index_sequence<sizeof...(Args)>{});
        // 调用结束后 delete 自己
        delete this;
    }

    // template <typename F, std::size_t... I>
    // void applyTuple(F& f, std::index_sequence<I...>) {
    //     f(std::get<I>(tpl)...);
    // }

    template <typename... Ts, std::size_t... I>
    void applyTuple(std::function<void(Ts...)>& f, std::index_sequence<I...>) {
        f(std::get<I>(tpl)...); // 这里的 std::get<I>(tpl) 会推导出 const char*, int 等
    }
};
