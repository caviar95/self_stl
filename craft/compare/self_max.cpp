#include <iostream>

namespace my_namespace {

template <typename T>
T max(T a, T b) {
    std::cout << "self defined max\n";

    return a > b ? a : b;
}


void Test() {
    std::cout << max(1, 2) << std::endl;
    std::cout << "global max\n";
    std::cout << std::max(1, 2) << std::endl;
}
}

int main() {
    my_namespace::Test();
    return 0;
}