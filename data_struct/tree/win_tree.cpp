#include <iostream>
#include <vector>
#include <climits>
#include <cassert>

class WinnerTree {
private:
    int k;
    std::vector<int> tree;   // 完全二叉树结构，1-based，tree[1] 是根，tree[0] 存储最终 winner 索引
    std::vector<int> leaves; // 叶子数组，存储实际数据

public:
    WinnerTree(const std::vector<int>& init) : k(init.size()), leaves(init) {
        // 树大小需要 2*k（包含叶子和内部节点）
        tree.resize(2 * k, -1);
        build();
    }

    void build() {
        // 初始化叶子节点（从 tree[k] 到 tree[2k - 1]）
        for (int i = 0; i < k; ++i) {
            tree[k + i] = i;
        }

        // 从底向上构建 winner tree
        for (int i = k - 1; i > 0; --i) {
            int left = tree[i * 2];
            int right = tree[i * 2 + 1];

            if (right == -1 || (left != -1 && leaves[left] <= leaves[right])) {
                tree[i] = left;
            } else {
                tree[i] = right;
            }
        }

        tree[0] = tree[1]; // winner 存到 tree[0]
    }

    void adjust(int leafIdx) {
        int pos = k + leafIdx;
        while (pos > 1) {
            int sibling = (pos % 2 == 0) ? pos + 1 : pos - 1;
            int parent = pos / 2;

            int left = tree[parent * 2];
            int right = (parent * 2 + 1 < (int)tree.size()) ? tree[parent * 2 + 1] : -1;

            if (right == -1 || (left != -1 && leaves[left] <= leaves[right])) {
                tree[parent] = left;
            } else {
                tree[parent] = right;
            }

            pos = parent;
        }

        tree[0] = tree[1];
    }

    int winner() const {
        return tree[0];
    }

    int value(int i) const {
        return leaves[i];
    }

    void popAndReplace(int idx, int newValue) {
        leaves[idx] = newValue;
        adjust(idx);
    }
};


int main() {
    std::vector<int> init = {3, 6, 1, 9};
    WinnerTree wt(init);

    for (int i = 0; i < 4; ++i) {
        int win = wt.winner();
        std::cout << wt.value(win) << " ";
        wt.popAndReplace(win, INT_MAX); // 替换为无穷大，模拟归并过程
    }

    return 0;
}
