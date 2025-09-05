#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <iostream>

using namespace std;

template <typename Base>
class AutoRegistry {
public:
    using Creator = function<unique_ptr<Base>()>;

    template <typename T>
    static void RegisterType() {
        static_assert(is_base_of<Base, T>::value, "Registered type must derive from Base");
        lock_guard<mutex> lk(GetMutex());
        GetCreator() = [] { return make_unique<T>(); };
    }

    static unique_ptr<Base> CreateDefault() {
        lock_guard<mutex> lk(GetMutex());
        if (!GetCreator()) {
            cerr << "[AutoRegistry] No type registered for default creation.\n";
            return nullptr;
        }
        return GetCreator()();
    }

private:
    static Creator& GetCreator() {
        static Creator creator;
        return creator;
    }

    static mutex& GetMutex() {
        static mutex mtx;
        return mtx;
    }
};

template <typename Base, typename Derived>
class AutoRegister {
public:
    AutoRegister() {
        AutoRegistry<Base>::template RegisterType<Derived>();
    }
};


#0 in ?? () from /lib64/libgcc_s.so.1
#1 in _Unwind_Backtrace () from /lib64/libgcc_s.so.1
#2 in backtrace () from /lib64/libgcc_s.so.1
#3 in CrashSignalProcess (signum=11)
#4 <signal handler called>
#5 in ?? ()
#6 in std::_Function_base::~_Function_base
#7 std::function<std::unique_ptr<ILogger, std::default_delete<ILogger>>()>::~function()
#8 in __run_exit_handlers () from /lib64/libc.so.6