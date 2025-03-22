#include "command_light.hpp"
#include "control.hpp"

#include "gtest/gtest.h"

TEST(CommandLight, base) {
    Light *light = new Light();

    CommandIntf *lightOn = new LightOnCommand(light);
    CommandIntf *lightOff = new LightOffCommand(light);

    RemoteControl *control = new RemoteControl();

    control->SetCommand(lightOn);
    control->Execute();

    control->SetCommand(lightOff);
    control->Execute();

    delete light;
    delete lightOn;
    delete lightOff;
    delete control;
}
