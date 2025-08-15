#pragma once

#include <unordered_map>
#include <cstdint>
#include <string>

class StateMachine {
public:
    enum DeviceState : uint32_t {
        STATE_A = 1 << 0,
        STATE_B = 1 << 1,
        STATE_C = 1 << 2,
        STATE_D = 1 << 3
    };

    StateMachine();

    void AddMapping(uint32_t mask, const std::string& finalState);

    std::string FindFinalState(uint32_t mask) const;

    bool CompareMasks(uint32_t mask1, uint32_t mask2) const;

    void PrintStateTable() const;

private:
    std::unordered_map<uint32_t, std::string> stateTable;
};
