#include "state_machine.h"
#include <iostream>

StateMachine::StateMachine() {
    // add default state machine
}

void StateMachine::AddMapping(uint32_t mask, const std::string& finalState) {
    stateTable[mask] = finalState;
}

std::string StateMachine::FindFinalState(uint32_t mask) const {
    auto it = stateTable.find(mask);
    if (it != stateTable.end()) {
        return it->second;
    }

    return "Unknown";
}

bool StateMachine::CompareMasks(uint32_t mask1, uint32_t mask2) const {
    return mask1 == mask2;
}

void StateMachine::PrintStateTable() const {
    std::cout << "State Table:\n";

    for (const auto& entry : stateTable) {
        std::cout << "Mask: 0x" << std::hex << entry.first << " -> Final State: " << entry.second << std::dec << std::endl;
    }
}
