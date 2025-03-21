#pragma once

#include <iostream>

#include "observer.h"

class SmsSubscriber : public Observer {
public:
    void update(const std::string& message) override {
        std::cout << "SMS Subscriber received a message: " << message << std::endl;
    }
};
