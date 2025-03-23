#pragma once

#include <iostream>

#include "command_intf.h"

class Light {
public:
    void TurnOn() {
        std::cout << "Light is on" << std::endl;
    }

    void TurnOff() {
        std::cout << "Light is off" << std::endl;
    }
};

class LightOnCommand : public CommandIntf {
private:
    Light *light;

public:
    LightOnCommand(Light *light) : light(light) {}

    void Execute() override {
        light->TurnOn();
    }
};

class LightOffCommand : public CommandIntf {
private:
    Light *light;

public:
    LightOffCommand(Light *light) : light(light) {}

    void Execute() override {
        light->TurnOff();
    }
};
