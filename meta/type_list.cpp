#include <iostream>
#include <type_traits>

// 辅助模板，用于出发编译时错误信息
template <typename>
struct always_false : std::false_type {};

// 类型列表容器
template <typename... Ts>
struct TypeList {};

// 主模板生命（处理所有未特化情况）
template <typename List>
struct GetList {
    static_assert(always_false<List>::value, "Type mismatch");
};

// 特化版本 处理非空列表
template <typename T, typename... Ts>
struct GetList<TypeList<T, Ts...>> {
    using type = T;
};

int main() {
    // 测试非空列表
    using NonEmptyList = TypeList<int, double, char>;
    static_assert(std::is_same<GetList<NonEmptyList>::type, int>::value, "Type mismatch");
    std::cout << "First Type: int\n";

    // 测试空列表（将触发编译错误）
    // using EmptyList = TypeList<>;
    // using TestType = GetList<EmptyList>::type; // 取消注释此行以查看错误

    return 0;
}
