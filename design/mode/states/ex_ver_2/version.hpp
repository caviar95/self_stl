#pragma once

#include "fsm.hpp"
#include <memory>
#include <stdexcept>

enum class Version {
    V1,
    V2
};

extern std::unique_ptr<IStateMachine> CreateStateMachineV1();
extern std::unique_ptr<IStateMachine> CreateStateMachineV2();

std::unique_ptr<IStateMachine> CreateStateMachine(Version version) {
    switch (version) {
        case Version::V1:
            return CreateStateMachineV1();
        case Version::V2:
            return CreateStateMachineV2();
        default:
            return nullptr;
    }
}