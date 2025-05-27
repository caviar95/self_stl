#pragma once

#include <vector>
#include <mutex>
#include <algorithm>

#include "subject.h"

class NewsPublisher : public Subject
{
public:
    void attach(std::shared_ptr<Observer> observer) override {
        std::lock_guard<std::mutex> lk(mtx_);
        observers_.push_back(observer);
    }

    void detach(std::shared_ptr<Observer> observer) override {
    std::lock_guard<std::mutex> lk(mtx_);

    observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
    // auto it = std::find_if(observers_.begin(), observers_.end(), 
    //     [&observer](const std::shared_ptr<Observer>& o) {
    //         return o == observer;
    //     });
    // if (it != observers_.end()) {
    //     observers_.erase(it);
    // }
}
    void notify() override {
        std::lock_guard<std::mutex> lk(mtx_);
        for (auto &observer : observers_) {
            observer->update(latest_news_);
        }
    }

    void publish(const std::string &news) {
        latest_news_ = news;
        notify();
    }

private:
    std::vector<std::shared_ptr<Observer>> observers_;
    std::string latest_news_;
    std::mutex mtx_;
};
