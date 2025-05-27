#pragma once

#include <vector>
#include <memory>
#include "observer_base.h"


template <typename DataType>
class Subject {
public:
    void RegisterObserver(std::shared_ptr<ObserverBase<DataType>> observer) {
        observers.emplace_back(observer);
    }

    void UnRegisterObserver(std::shared_ptr<ObserverBase<DataType>> observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void NotifyObservers(OperType oper, const DataType &data) {
        for (auto &observer : observers) {
            observer->update(oper, data);
        }
    }

private:
    std::vector<std::shared_ptr<ObserverBase<DataType>>> observers;
};
