#pragma once

#include "mask_state_machine.h"
#include "subsystem_states.h"

class MyStateMachine : public MaskStateMachine<2> {
public:
    using Base = MaskStateMachine<2>;

    MyStateMachine() {}
};
