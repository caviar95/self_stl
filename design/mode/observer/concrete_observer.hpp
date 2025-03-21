#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>

#include "event.hpp"
#include "observer.h"
#include "subject.h"

class AdvancedObserver : public Observer {
public:
    void update(const std::string& message) override {}

    virtual void onEvent(const Event& event) = 0;
};

class AdvancedPublisher : public Subject {
public:
    void subscribe(const std::string& eventType, std::shared_ptr<AdvancedObserver> observer) {
        event_observers_[eventType].push_back(observer);
    }

    void publishEvent(const Event& event) {
        auto it = event_observers_.find(event.type());
        if (it != event_observers_.end()) {
            for (const auto& observer : it->second) {
                observer->onEvent(event);
            }
        }
    }

private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<AdvancedObserver>>> event_observers_;
};

class AdFilter : public AdvancedObserver {
public:
    void onEvent(const Event& event) override {
        if (event.type() == "ad") {
            const auto &ad = static_cast<const AdEvent&>(event);
            std::cout << "Block Ad: " << ad.content() << std::endl;
        }
    }
};