#include <iostream>
#include <functional>

auto make_counter(int start, int step) {
    int count = start;
    return [count, step]() mutable -> int {
        int current = count;
        count += step;
        return current;
    };
}

int main() {
    auto counter = make_counter(0, 2);

    for (int i = 0; i < 5; ++i) {
        std::cout << counter() << " ";  // 0 2 4 6 8
    }
    std::cout << "\n";

    return 0;
}