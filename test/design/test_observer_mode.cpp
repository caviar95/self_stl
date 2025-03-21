#include "news_publisher.hpp"
#include "sms_subscriber.hpp"
#include "email_subscriber.hpp"

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
