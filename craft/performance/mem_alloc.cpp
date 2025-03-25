#include <iostream>
#include <string>
#include <chrono>

// 未优化：频繁重新分配
std::string buildStringBad(int n) {
    std::string s;
    for (int i = 0; i < n; ++i) {
        s += 'a' + (i % 26);
    }
    return s;
}

// 优化：预分配内存
std::string buildStringGood(int n) {
    std::string s;
    s.reserve(n); // 关键优化
    for (int i = 0; i < n; ++i) {
        s += 'a' + (i % 26);
    }
    return s;
}

int main() {
    const int N = 10'000'000;

    auto start = std::chrono::high_resolution_clock::now();
    buildStringBad(N);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Bad: " << diff.count() << "s\n";

    start = std::chrono::high_resolution_clock::now();
    buildStringGood(N);
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << "Good: " << diff.count() << "s\n";

    return 0;
}
