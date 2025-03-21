#include "news_publisher.hpp"
#include "sms_subscriber.hpp"
#include "email_subscriber.hpp"

#include "event_mgr.hpp"

#include "gtest/gtest.h"

TEST(Observer, news) {
    auto publisher = std::make_shared<NewsPublisher>();
    auto emailSub = std::make_shared<EmailSubscriber>();
    auto smsSub = std::make_shared<SmsSubscriber>();

    publisher->attach(emailSub);
    publisher->attach(smsSub);

    publisher->publish("C++17 is officialally released!");

    publisher->detach(emailSub);
    publisher->publish("C++20 feature complete!");
}

void observer_func1() {
    std::cout << "observer_func1 is notified\n";
}

void observer_func2() {
    std::cout << "observer_func2 is notified\n";
}

// struct Initializer {
//     Initializer() {
//         EventMgr::GetInstance().Subscribe(observer_func1);
//         EventMgr::GetInstance().Subscribe(observer_func2);
//     }
// };

namespace {

auto initializer = []() {
    EventMgr::GetInstance().Subscribe(observer_func1);
    EventMgr::GetInstance().Subscribe(observer_func2);
    return 0;
}();

}

// static Initializer initializer; // 确保Initializer 在程序启动时被构造

TEST(Observer, event_mgr) {
    EventMgr::GetInstance().Publish();
}