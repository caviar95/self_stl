#pragma once

#include <vector>
#include <ranges>
#include <numeric>

class UnionFind {
private:
    std::vector<int> fa;
    int cnt; // 连通块个数

public:

    UnionFind(int n) : fa(n), cnt(n) {
        // 非C++标准库函数，导致编译报错。
        std::ranges::iota(fa, 0);

        // for (int i = 0; i < n; ++i) {
        //     fa[i] = i;
        // }
    }

    int find(int x) {
        while (fa[x] != x) {
            x = fa[x];
        }

        return x;
    }

    void merge(int p, int q) {
        int x = find(p);
        int y = find(q);

        if (x == y) {
            return;
        }

        fa[x] = y;
        cnt--;
    }

    int count() const {
        return cnt;
    }

    bool connected(int p, int q) {
        int x = find(p);
        int y = find(q);
        return x == y;
    }
};
