#include <iostream>

struct A {
    int x;
    int y;

    A(int a, int b) : x(a), y(b) {}
};

bool operator==(const A& a, const A& b) {
    return a.x == b.x && a.y == b.y;
}

struct B {
    int x;
    int y;
    
    B(int a, int b) : x(a), y(b) {}

    bool operator==(const B& b) const {
        return x == b.x && y == b.y;
    }
};

void Test() {
    A a(1, 2);
    A b(1, 2);

    if (a == b) {
        std::cout << "Equal" << std::endl;
    }
}


void TestB() {
    B b(1, 2);
    B c(1, 2);

    if (b == c) {
        std::cout << "Equal" << std::endl;
    }
}

template<typename T>
T max(T a, T b) { return b < a ? a : b; }
int max(int a, int b) { return a > b ? a : b; }
int main() {
    std::cout << max(5, 10) << " ";
    std::cout << max<>(5, 10) << std::endl;
    return 0;
}

// int main() {
//     Test();

//     TestB();

//     return 0;
// }