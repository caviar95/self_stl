#pragma once

#include <iostream>

#include "observer.h"

class EmailSubscriber : public Observer {
public:
    void update(const std::string &message) override {
        std::cout << "EmailSubscriber: " << message << std::endl;
    }
};
