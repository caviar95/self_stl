#include <iostream>
#include <vector>
#include <array>
#include <stdexcept>
#include <initializer_list>

constexpr uint32_t BIT(int n) { return 1u << n; }

enum class TotalState {
    Startup,
    Working,
    Finish
};

constexpr uint32_t Mask(std::initializer_list<int> bits) {
    uint32_t m = 0;
    for (auto b : bits) {
        m |= BIT(b);
    }

    return m;
}

bool CanTransition(uint32_t cur, uint32_t next) {
    if (cur == next) {
        return true;
    }

    if ((next & cur) != cur) {
        return false;
    }

#if defined(__GNUG__)
    auto lowestBitIndex = [](uint32_t x) {
        return __builtin_ctz(x);
    };
#else
    auto lowestBitIndex = [](uint32_t x) {
        for (int i = 0; i < 32; ++i) {
            if (x & (1u << i)) {
                return i;
            }
        }

        return 32;
    };
#endif

    int curLow = lowestBitIndex(cur);
    int nextLow = lowestBitIndex(next);

    return nextLow >= curLow;
}


