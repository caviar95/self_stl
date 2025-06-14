#include <iostream>
#include <tuple>
#include <string>

struct MyStruct {
    int a;
    double b;
    std::string c;
};

auto as_tuple(const MyStruct& s) {
    return std::tie(s.a, s.b, s.c);
}

int main() {
    MyStruct s{1, 2.5, "hello"};
    std::apply([](auto&&... args) {
        ((std::cout << args << " "), ...);
    }, as_tuple(s));
}
