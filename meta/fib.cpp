#include <iostream>

template <int N>
struct Fibonacci {
    static constexpr int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template <>
struct Fibonacci<0> {
    static constexpr int value = 0;
};

template <>
struct Fibonacci<1> {
    static constexpr int value = 1;
};

int main() {
    std::cout << "Fibonacci(5) = " << Fibonacci<5>::value << std::endl;

    static_assert(Fibonacci<10>::value == 55, "Compile-time check");

    return 0;
}