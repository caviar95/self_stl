#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;
using namespace chrono;

class Solution {
public:
    using PII = pair<int, int>;
    using Point = pair<int, int>;

    int gcd(int a, int b) {
        return b == 0 ? abs(a) : gcd(b, a % b);
    }

    int countTrapezoids(vector<vector<int>>& pointsVec) {
        vector<Point> velmoranic;
        for (auto& p : pointsVec)
            velmoranic.emplace_back(p[0], p[1]);

        int n = velmoranic.size();
        unordered_map<PII, vector<pair<int, int>>, hash_pair> slopeMap;

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                int dx = velmoranic[j].first - velmoranic[i].first;
                int dy = velmoranic[j].second - velmoranic[i].second;
                if (dx == 0) {
                    dy = 1;
                    dx = 0;
                } else if (dy == 0) {
                    dy = 0;
                    dx = 1;
                } else {
                    int g = gcd(dx, dy);
                    dx /= g;
                    dy /= g;
                    if (dx < 0) {
                        dx = -dx;
                        dy = -dy;
                    }
                }
                slopeMap[{dy, dx}].emplace_back(i, j);
            }
        }

        long long res = 0;
        for (auto& [slope, segments] : slopeMap) {
            int m = segments.size();
            for (int i = 0; i < m; ++i) {
                auto [a1, b1] = segments[i];
                for (int j = i + 1; j < m; ++j) {
                    auto [a2, b2] = segments[j];
                    unordered_set<int> used = {a1, b1, a2, b2};
                    if (used.size() == 4)
                        res++;
                }
            }
        }

        return (int)res;
    }

private:
    struct hash_pair {
        size_t operator()(const pair<int, int>& p) const {
            return hash<long long>()(((long long)p.first) << 32 | (unsigned int)p.second);
        }
    };
};

int main() {
    Solution sol;
    const int N = 500;
    vector<vector<int>> points;

    // 构造最大量级输入（500个不同点）
    for (int i = 0; i < N; ++i) {
        points.push_back({i, (i * i + 37) % 1000});
    }

    // 计时开始
    auto start = high_resolution_clock::now();
    int result = sol.countTrapezoids(points);
    auto end = high_resolution_clock::now();

    // 计算耗时
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Result: " << result << endl;
    cout << "Execution time: " << duration.count() << " ms" << endl;

    return 0;
}
