#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include <algorithm>
#include <string>

// 定义事件类型
enum class EventType {
    UserRegistered,
    OrderPlaced,
    StockUpdated,
};

// 观察者接口
template <typename DataType>
class Observer {
public:
    virtual void OnEvent(EventType event, const DataType& data) = 0;
    virtual ~Observer() = default;
};

// 事件总线（发布者-订阅者机制）
class EventBus {
public:
    // 注册订阅者
    template <typename DataType>
    void Subscribe(EventType event, std::shared_ptr<Observer<DataType>> observer) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // 确保每个观察者仅被注册一次
        auto& observers = eventObservers_[event];
        if (std::find(observers.begin(), observers.end(), observer) == observers.end()) {
            observers.push_back(observer);
        }
    }

    // 注销订阅者
    template <typename DataType>
    void Unsubscribe(EventType event, std::shared_ptr<Observer<DataType>> observer) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto& observers = eventObservers_[event];
        auto it = std::find(observers.begin(), observers.end(), observer);
        if (it != observers.end()) {
            observers.erase(it);
        }
    }

    // 发布事件
    template <typename DataType>
    void Publish(EventType event, const DataType& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = eventObservers_.find(event);
        if (it != eventObservers_.end()) {
            // 只有匹配的观察者会被调用
            for (const auto& observer : it->second) {
                std::shared_ptr<Observer<DataType>> specificObserver = std::static_pointer_cast<Observer<DataType>>(observer);
                specificObserver->OnEvent(event, data);
            }
        }
    }

private:
    std::unordered_map<EventType, std::vector<std::shared_ptr<void>>> eventObservers_;
    std::mutex mutex_;
};

// 用户类
class User {
public:
    User(const std::string& name) : name_(name) {}

    const std::string& GetName() const {
        return name_;
    }

private:
    std::string name_;
};

// 订单类
class Order {
public:
    Order(int orderId, const std::string& product) : orderId_(orderId), product_(product) {}

    int GetOrderId() const {
        return orderId_;
    }

    const std::string& GetProduct() const {
        return product_;
    }

private:
    int orderId_;
    std::string product_;
};

// 观察者实现（例如：用户管理模块）
class UserManager : public Observer<User> {
public:
    void OnEvent(EventType event, const User& user) override {
        if (event == EventType::UserRegistered) {
            std::cout << "User Registered: " << user.GetName() << std::endl;
        }
    }
};

// 观察者实现（例如：订单管理模块）
class OrderManager : public Observer<Order> {
public:
    void OnEvent(EventType event, const Order& order) override {
        if (event == EventType::OrderPlaced) {
            std::cout << "Order Placed: " << order.GetOrderId() << " for product " << order.GetProduct() << std::endl;
        }
    }
};

// 观察者实现（例如：库存管理模块）
class StockManager : public Observer<Order> {
public:
    void OnEvent(EventType event, const Order& order) override {
        if (event == EventType::OrderPlaced) {
            std::cout << "Updating stock for product: " << order.GetProduct() << std::endl;
        }
    }
};

// 示例程序
int main() {
    EventBus eventBus;

    // 创建模块（观察者）
    auto userManager = std::make_shared<UserManager>();
    auto orderManager = std::make_shared<OrderManager>();
    auto stockManager = std::make_shared<StockManager>();

    // 创建用户和订单
    User user("Alice");
    Order order(123, "Laptop");

    // 注册观察者
    eventBus.Subscribe<User>(EventType::UserRegistered, userManager);
    eventBus.Subscribe<Order>(EventType::OrderPlaced, orderManager);
    eventBus.Subscribe<Order>(EventType::OrderPlaced, stockManager);

    // 发布事件
    eventBus.Publish<User>(EventType::UserRegistered, user);
    eventBus.Publish<Order>(EventType::OrderPlaced, order);

    // 注销观察者
    eventBus.Unsubscribe<Order>(EventType::OrderPlaced, stockManager);

    // 重新发布事件，库存管理模块不再收到通知
    eventBus.Publish<Order>(EventType::OrderPlaced, order);

    return 0;
}
