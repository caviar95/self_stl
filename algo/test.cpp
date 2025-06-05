#include "union_find_v2.hpp"

#include <iostream>

using std::cout;
using std::endl;

int main() {
    UnionFind uf(10);  // 初始化10个元素（0~9）

    // 合并一些集合
    uf.Unite(1, 2);
    uf.Unite(3, 4);
    uf.Unite(2, 4);  // 1-2-3-4 成为一个集合
    uf.Unite(5, 6);
    uf.Unite(6, 7);

    // 测试连通性
    cout << std::boolalpha;  // 输出 true/false 而不是 1/0
    cout << "1 and 4 connected? " << uf.Connected(1, 4) << endl; // true
    cout << "1 and 5 connected? " << uf.Connected(1, 5) << endl; // false

    // 再合并两个集合
    uf.Unite(4, 5);  // 现在 1-2-3-4-5-6-7 连通
    cout << "1 and 6 connected? " << uf.Connected(1, 6) << endl; // true

    // 自连接测试
    cout << "Is 9 connected to itself? " << uf.Connected(9, 9) << endl; // true
    cout << "Is 9 connected to 0? " << uf.Connected(9, 0) << endl; // false

    return 0;
}
