#include <iostream>
#include <string>
#include <utility>
#include <tuple>

// ---------------------------
// 工具函数：for_each_field（仅适用于字段数量固定的聚合类型）
// ---------------------------
template <typename T, typename F>
void for_each_field(T& obj, F&& func) {
    // 对于 MyStruct 有 3 个字段，手动结构化绑定
    auto& [f0, f1, f2] = obj;
    func(f0);
    func(f1);
    func(f2);
}

// ---------------------------
// 示例结构体
// ---------------------------
struct MyStruct {
    int id;
    float score;
    std::string name;
};

int main() {
    MyStruct s{42, 98.5f, "Alice"};

    for_each_field(s, [](auto& field) {
        std::cout << field << std::endl;
    });
}
