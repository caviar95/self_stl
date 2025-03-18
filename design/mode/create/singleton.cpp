#include <iostream>
#include <memory>
#include <mutex>

class Singleton {
public:
    // 禁用拷贝构造函数和赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    // 提供全局访问点
    static Singleton& getInstance() {
        // 第一次检查，无需锁定
        if (!instance_) {
            // 使用std::call_once来确保只初始化一次
            std::call_once(initFlag_, &Singleton::initInstance);
        }
        // 由于使用了std::call_once，这里实际上不需要第二次检查，但为了与双重检查锁定的概念保持一致，可以保留这个检查（尽管它是多余的）
        // 注意：在真实场景中，如果去掉这个检查并且initInstance函数内部有复杂的逻辑（尽管它不应该有），理论上仍然存在极小的时间窗口问题，
        // 但由于std::call_once的保证，这里实际上是不会发生的。因此，这个检查主要是为了演示目的而保留。
        return *instance_;
    }

private:
    // 私有构造函数，防止外部实例化
    Singleton() {
        std::cout << "Singleton instance created." << std::endl;
    }

    // 静态成员变量存储实例指针， 使用指针是为了延迟静态初始化，避免在程序启动时立即分配内存
    static std::unique_ptr<Singleton> instance_;

    // 静态标志变量，用于std::call_once
    static std::once_flag initFlag_;

    // 静态初始化函数
    static void initInstance() {
        instance_ = std::make_unique<Singleton>();
    }
};

// 初始化静态成员变量
std::unique_ptr<Singleton> Singleton::instance_ = nullptr;
std::once_flag Singleton::initFlag_;

int main() {
    // 获取单例实例并调用其方法（如果有的话）
    Singleton& singleton = Singleton::getInstance();
    // ... 使用singleton实例 ...

    return 0;
}