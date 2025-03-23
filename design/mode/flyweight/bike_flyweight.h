#pragma once

#include <string>

class BikeFlyWeight {
protected:
    int state{};

public:
    virtual void ride(std::string userName) = 0;
    virtual void back() = 0;

    int getState() const {
        return state;
    }
};

class MoBikeF