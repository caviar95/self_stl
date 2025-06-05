#include <vector>
#include <numeric>

using std::vector;
using std::iota;

class UnionFind {
public:
    UnionFind(int n) : parent_(n), rank_(n, 1) {
        iota(parent_.begin(), parent_.end(), 0);
    }

    int Find(int x) {
        if (parent_[x] != x) {
            parent_[x] = Find(parent_[x]);
        }

        return parent_[x];
    }

    void Unite(int x, int y) {
        int px = Find(x);
        int py = Find(y);
        if (px == py) {
            return;
        }

        if (rank_[px] < rank_[py]) {
            parent_[px] = py;
        } else if (rank_[px] > rank_[py]) {
            parent_[py] = px;
        } else {
            parent_[py] = px;
            rank_[px]++;
        }
    }

    bool Connected(int x, int y) {
        return Find(x) == Find(y);
    }

private:
    vector<int> parent_;
    vector<int> rank_;
};
