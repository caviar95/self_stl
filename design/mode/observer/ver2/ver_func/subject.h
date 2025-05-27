#pragma once


#include "observer.h"

template <typename DataType>
class Subject {
public:
    void RegisterObserver(OperType op, std::function<void(const DataType&)> func) {
        observers[op].emplace_back(func);
    }

    void UnregisterObserver(OperType op, std::function<void(const DataType&)> func) {
        observers[op].erase(std::remove(observers[op].begin(), observers[op].end(), func), observers[op].end());
    }

    void NotifyObservers(OperType op, const DataType &data) {
        for (auto &func : observers[op]) {
            func(data);
        }
    }

private:
    std::unordered_map<OperType, std::vector<std::function<void(DataType)>> observers;
};
