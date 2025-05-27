#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <iostream>
#include <algorithm>
#include <memory>

// 定义操作类型，描述每个模块可以执行的操作
enum class OperType {
    Add,
    Remove,
    Update,
    Delete
};

// 定义模块类型，模块ID由枚举类型定义
enum class ModuleID {
    ModuleA,
    ModuleB,
    ModuleC
};

// 为 Enum 类型定义哈希函数
namespace std {
    template <>
    struct hash<ModuleID> {
        size_t operator()(const ModuleID& id) const {
            return static_cast<size_t>(id);  // 将 enum 转换为整数进行哈希
        }
    };

    template <>
    struct hash<OperType> {
        size_t operator()(const OperType& op) const {
            return static_cast<size_t>(op);  // 将 enum 转换为整数进行哈希
        }
    };
}

// 观察者类模板
template <typename DataType>
class Observer {
public:
    virtual void Update(const DataType& data) = 0;  // 观察者的更新接口
};

// 具体的观察者实现，处理某个模块的特定行为
template <typename DataType>
class ConcreteObserver : public Observer<DataType> {
public:
    ConcreteObserver(const std::string& name, std::function<void(const DataType&)> callback)
        : name(name), callback(callback) {}

    void Update(const DataType& data) override {
        std::cout << "Observer " << name << " received update: ";
        callback(data);
    }

private:
    std::string name;
    std::function<void(const DataType&)> callback;
};

// 主题类（Subject），处理观察者注册、注销和通知
template <typename DataType>
class Subject {
public:
    using ObserverPtr = std::shared_ptr<Observer<DataType>>;  // 使用智能指针管理观察者
    using ObserverWeakPtr = std::weak_ptr<Observer<DataType>>;  // 使用 weak_ptr 避免循环引用

    // 注册观察者，避免重复注册
    void RegisterObserver(ModuleID moduleId, OperType operType, ObserverPtr observer) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto& observerList = observers[moduleId][operType];
        
        // 检查是否已注册该观察者
        if (std::find(observerList.begin(), observerList.end(), observer) == observerList.end()) {
            observerList.emplace_back(observer);
        } else {
            std::cout << "Observer already registered for this operation.\n";
        }
    }

    // 注销观察者
    void UnregisterObserver(ModuleID moduleId, OperType operType, ObserverPtr observer) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto& observerList = observers[moduleId][operType];
        
        // 使用自定义的比较器来删除观察者
        observerList.erase(
            std::remove_if(observerList.begin(), observerList.end(),
                [&observer](const ObserverWeakPtr& weakObserver) {
                    // 解锁 weak_ptr，比较 shared_ptr 是否相同
                    if (auto sp = weakObserver.lock()) {
                        return sp == observer;  // 比较 shared_ptr 对象
                    }
                    return false;  // 如果 weak_ptr 已经过期，不删除
                }),
            observerList.end());

        // 如果观察者列表为空，清理操作类型
        if (observerList.empty()) {
            observers[moduleId].erase(operType);
        }

        // 如果模块没有任何操作，清理模块
        if (observers[moduleId].empty()) {
            observers.erase(moduleId);
        }
    }

    // 延迟通知：通过任务队列实现通知，避免死锁
    void NotifyObservers(ModuleID moduleId, OperType operType, const DataType& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // 将通知操作放到队列中
        std::vector<ObserverWeakPtr> notifyList;
        for (const auto& observer : observers[moduleId][operType]) {
            notifyList.push_back(observer);
        }

        // 不持有锁，开始通知
        for (auto& weakObserver : notifyList) {
            if (auto observer = weakObserver.lock()) {
                observer->Update(data);
            }
        }
    }

private:
    // 使用 weak_ptr 来避免观测者被销毁时导致的悬空指针
    std::unordered_map<ModuleID, std::unordered_map<OperType, std::vector<ObserverWeakPtr>>> observers;
    std::mutex mutex_;  // 用于线程安全的保护
};

// 测试代码：使用观察者模式
int main() {
    // 创建主题类对象，分别管理不同类型的数据
    Subject<std::string> stringSubject;
    Subject<int> intSubject;

    // 创建观察者
    auto stringObserver = std::make_shared<ConcreteObserver<std::string>>(
        "StringObserver", [](const std::string& data) { std::cout << "String Data: " << data << std::endl; });
    auto intObserver = std::make_shared<ConcreteObserver<int>>(
        "IntObserver", [](const int& data) { std::cout << "Int Data: " << data << std::endl; });

    // 注册观察者到模块A的Add操作
    stringSubject.RegisterObserver(ModuleID::ModuleA, OperType::Add, stringObserver);
    intSubject.RegisterObserver(ModuleID::ModuleB, OperType::Update, intObserver);

    // 再次注册相同的观察者，应该被拒绝
    stringSubject.RegisterObserver(ModuleID::ModuleA, OperType::Add, stringObserver);  // 重复注册

    // 通知观察者
    stringSubject.NotifyObservers(ModuleID::ModuleA, OperType::Add, "New String Item");
    intSubject.NotifyObservers(ModuleID::ModuleB, OperType::Update, 100);

    // 注销观察者
    stringSubject.UnregisterObserver(ModuleID::ModuleA, OperType::Add, stringObserver);
    intSubject.UnregisterObserver(ModuleID::ModuleB, OperType::Update, intObserver);

    return 0;
}
