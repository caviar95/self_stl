#pragma once

#include <cstdint>
#include <iostream>

using Mask = uint64_t;


class MaskStateMachine {
public:
    static constexpr int SUBSYSTEMS = 4;
    static constexpr int BITS_PER_SYS = 16;

    MaskStateMachine() {
        currentMask_ = 0;
    }

    uint16_t GetSusSystemState(int idx) const {
        if (idx < 0 || idx >= SUBSYSTEMS) {
            return UINT16_MAX;
        }

        return (currentMask_ >> (idx * BITS_PER_SYS)) & 0xFF;
    }

    void SetSubsystemState(int idx, uint16_t state) {
        if (idx < 0 || idx >= SUBSYSTEMS) {
            return;
        }

        Mask clearMask = ~(Mask(0xFF) << (idx * BITS_PER_SYS));
        currentMask_ &= clearMask;

        currentMask_ |= (Mask(state) << (idx * BITS_PER_SYS));
    }

    Mask GetMask() const {
        return currentMask_;
    }

    void Print() const {
        std::cout << "Current Mask: 0x" << std::hex << currentMask_ << "[";

        for (int i = 0; i < SUBSYSTEMS; ++i) {
            std::cout << GetSusSystemState(i);
            if (i < SUBSYSTEMS - 1) {
                std::cout << ",";
            }
        }

        std::cout << "]" << std::dec << std::endl;
    }

private:
    Mask currentMask_;
};