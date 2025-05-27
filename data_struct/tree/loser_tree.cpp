#include <iostream>
#include <vector>
#include <climits>

class LoserTree {
private:
    int k;
    std::vector<int> tree;   // 内部节点，size k，tree[0]为胜者叶子编号
    std::vector<int> leaves; // 叶子值，size k

public:
    explicit LoserTree(const std::vector<int>& input) : k((int)input.size()), tree(k, -1), leaves(input) {
        build();
    }

    void build() {
        // 初始化所有内部节点为-1
        for (int i = 0; i < k; ++i) tree[i] = -1;

        for (int i = k - 1; i >= 0; --i) {
            adjust(i);
        }
    }

    void adjust(int s) {
        int t = s;
        int parent = (s + k) / 2;

        while (parent > 0) {
            if (parent >= k) {
                std::cerr << "ERROR: parent index out of range: " << parent << std::endl;
                std::cerr << "k = " << k << std::endl;
                std::exit(1);
            }

            // debug输出当前比较的节点和值
            // std::cout << "Adjust: parent = " << parent << ", t = " << t
            //           << ", leaves[t] = " << leaves[t]
            //           << ", tree[parent] = " << tree[parent]
            //           << ", leaves[tree[parent]] = " << (tree[parent] == -1 ? INT_MAX : leaves[tree[parent]])
            //           << std::endl;

            if (tree[parent] == -1 || leaves[t] < leaves[tree[parent]]) {
                std::swap(t, tree[parent]);
            }
            parent /= 2;
        }
        tree[0] = t;
    }

    int winner() const { return tree[0]; }
    int value(int idx) const {
        if (idx < 0 || idx >= k) return INT_MAX;
        return leaves[idx];
    }

    void popAndReplace(int idx, int newVal) {
        if (idx < 0 || idx >= k) {
            std::cerr << "popAndReplace: idx out of range: " << idx << std::endl;
            return;
        }
        leaves[idx] = newVal;
        adjust(idx);
    }
};

int main() {
    std::vector<int> input = {20, 15, 30, 10};
    LoserTree lt(input);

    std::cout << "Winner index: " << lt.winner() << std::endl;
    std::cout << "Winner value: " << lt.value(lt.winner()) << std::endl;

    lt.popAndReplace(lt.winner(), 25);

    std::cout << "After replacement:" << std::endl;
    std::cout << "Winner index: " << lt.winner() << std::endl;
    std::cout << "Winner value: " << lt.value(lt.winner()) << std::endl;

    return 0;
}
