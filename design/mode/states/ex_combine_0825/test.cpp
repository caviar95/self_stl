#include "mask_state_machine.hpp"

int main() {
    constexpr size_t N = 4;

    std::array<int, N> width = {8, 8, 8, 8};

    MaskStateMachine<N> m(width);

    using CS = CombinedState<N>;
    using FS = FinalState<N>;

    CS finalAMask = CS::FromArray({{0x1, 0x0, 0xFF, 0x0}});

    m.RegisterFinal(FS("FinalA", finalAMask, 10));

    CS finalBMask = CS::FromArray({{0x2, 0x4, 0x0, 0x0}});

    m.RegisterFinal(FS("FinalB", finalBMask, 5));

    CS cur = CS::FromArray({{0x3, 0x4, 0x1, 0x0}});

    auto matches = m.MatchFinalStates(cur, "contains");

    std::cout << "Matched finals (contains):\n";

    for (const auto& f : matches) {
        std::cout << " " << f.name << " (prio=" << f.priority << ") mask=" << f.mask.ToString() << "\n";
    }

    std::vector<CS> set{
        CS::FromArray({{0x1, 0x0, 0x0, 0x0}}),
        CS::FromArray({{0x3, 0x1, 0x0, 0x0}}),
        CS::FromArray({{0x2, 0xFF, 0x1, 0x0}})
    };

    auto highest = m.HighestOfSet(set);

    if (highest) {
        std::cout << "Highest: " << highest->ToString() << "\n";
    }

    return 0;
}
