#include <vector>
#include <chrono>
#include <iostream>

void byRef(std::vector<int>& out) {
    out = {1, 2, 3, 4, 5};  // 触发赋值操作
}

std::vector<int> byRVO() {
    return {1, 2, 3, 4, 5};  // 理想情况下 RVO
}

int main() {
    const int N = 1'000'000;

    // 测试传引用
    auto start1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        std::vector<int> v;
        byRef(v);  // 每次构造新对象 + 赋值
    }
    auto end1 = std::chrono::high_resolution_clock::now();

    // 测试 RVO
    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        std::vector<int> v = byRVO();  // 依赖 RVO
    }
    auto end2 = std::chrono::high_resolution_clock::now();

    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);

    std::cout << "传引用耗时: " << duration1.count() << "ms\n";
    std::cout << "RVO耗时: " << duration2.count() << "ms\n";
}