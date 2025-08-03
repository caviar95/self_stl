#include <iostream>

struct Adder {
    Adder(int n) : n_(n) {}
    int operator()(int x) const {
        return x + n_;
    }

private:
    int n_;
};


void test_adder() {
    Adder add_five(5);
    int result = add_five(10); // Should return 15
    // You can add assertions or print statements to verify the result
    std::cout << "Result of adding 5 to 10: " << result << std::endl; // Expected output: 15
}

int main() {
    test_adder();
    return 0;
}