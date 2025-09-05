#pragma once

#include <cstdint>
#include <iostream>

using MaskType = uint64_t;

class MaskStateMachine {
public:

    explicit MaskStateMachine(MaskType init = 0) : state_(init) {}

    void SetState(uint32_t bit) {
        state_ |= (MaskType(1) << bit);
    }

    void ClearState(uint32_t bit) {
        state_ &= ~(MaskType(1) << bit);
    }

    bool HasState(uint32_t bit) const {
        return (state_ & (MaskType(1) << bit)) != 0;
    }

    MaskType GetState() const {
        return state_;
    }

    bool CanTransitionTo(uint32_t bit) const {
        for (int i = 0; i < bit; ++i) {
            if (!HasState(i)) {
                return false;
            }
        }

        return true;
    }

private:
    MaskType state_;
};

