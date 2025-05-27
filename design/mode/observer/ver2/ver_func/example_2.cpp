#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <functional>
#include <mutex>

enum class OperType { Add, Remove, Update };

template <typename DataType>
class Observer {
public:
    virtual void Update(const DataType& data) = 0;
    virtual ~Observer() = default;
};

template <typename DataType>
class ModuleManager {
public:
    using ObserverPtr = std::shared_ptr<Observer<DataType>>;

    // Register an observer to be notified when an operation occurs
    void RegisterObserver(OperType operType, ObserverPtr observer) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (std::find(observers[operType].begin(), observers[operType].end(), observer) == observers[operType].end()) {
            observers[operType].push_back(observer);
        }
    }

    // Unregister an observer
    void UnregisterObserver(OperType operType, ObserverPtr observer) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto& observerList = observers[operType];
        observerList.erase(std::remove(observerList.begin(), observerList.end(), observer), observerList.end());
    }

    // Notify all observers for a specific operation
    void NotifyObservers(OperType operType, const DataType& data) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto& observerList = observers[operType];
        for (auto& observer : observerList) {
            observer->Update(data);
        }
    }

    // Simulate an operation in the module manager
    void PerformOperation(OperType operType, const DataType& data) {
        // Logic for performing the operation (e.g., adding, removing, updating an object)
        // Then notify the observers
        NotifyObservers(operType, data);
    }

private:
    std::unordered_map<OperType, std::vector<ObserverPtr>> observers;  // Observers registered by operation type
    std::mutex mutex_;  // Protects observer list from race conditions
};

class StringObserver : public Observer<std::string> {
public:
    void Update(const std::string& data) override {
        std::cout << "StringObserver received data: " << data << std::endl;
    }
};

class IntegerObserver : public Observer<int> {
public:
    void Update(const int& data) override {
        std::cout << "IntegerObserver received data: " << data << std::endl;
    }
};

int main() {
    // Create the ModuleManager for a string data type
    ModuleManager<std::string> stringModuleManager;
    auto stringObserver = std::make_shared<StringObserver>();

    // Register the observer to the module manager
    stringModuleManager.RegisterObserver(OperType::Add, stringObserver);

    // Perform an operation and notify observers
    stringModuleManager.PerformOperation(OperType::Add, "New object added");

    // Unregister the observer
    stringModuleManager.UnregisterObserver(OperType::Add, stringObserver);
    
    // Perform another operation, but no observers should be notified
    stringModuleManager.PerformOperation(OperType::Add, "This should not be received by observers");

    return 0;
}
