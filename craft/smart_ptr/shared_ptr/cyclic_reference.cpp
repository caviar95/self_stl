#include <iostream>
#include <memory>


struct B;  // 前向声明

struct A {
    std::shared_ptr<B> b;
    ~A() { std::cout << "A destroyed\n"; }
};

struct B {
    // std::shared_ptr<A> a;
    std::weak_ptr<A> a;
    ~B() { std::cout << "B destroyed\n"; }
};

void leak_example() {
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();
    a->b = b;
    b->a = a;

}

int main() {
    leak_example();

    std::unique_ptr<int> a = std::make_unique<int>();

    return 0;
}