#include <gtest/gtest.h>
#include "state_machine_derived.h"

TEST(MaskStateMachineTest, MatchRequiredForbidden) {
    MyStateMachine sm;
    FinalState<2> fs;
    fs.name = "Idle+Active";
    fs.required.bits[0].set(S1_IDLE);
    fs.required.bits[1].set(S2_ACTIVE);
    sm.addFinalState(fs);

    // sm.setSubsystemState(0, S1_IDLE);
    // sm.setSubsystemState(1, S2_ACTIVE);

    std::string last;
    sm.onFinalChanged([&last](const std::string& name) {
        last = name;
    });
    sm.setSubsystemState(0, S1_IDLE);
    sm.setSubsystemState(1, S2_ACTIVE);

    EXPECT_EQ(last, "Idle+Active");
}
