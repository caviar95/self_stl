// #include <bits/stdc++.h>
#include <string>
#include <array>
#include <algorithm>
#include <vector>
#include <iostream>

using namespace std;

static inline bool betterStr(const string &a, const string &b) {
    if (b.empty()) {
        return true;
    }
    if (a.size() != b.size()) {
        return a.size() < b.size();
    }
    return a < b;
}

class Solution {
public:
    long long specialPalindrome(long long n) {
        string nstr = to_string(n);
        int len_n = (int)nstr.size();
        long long thomeralex = n; // midway 存储输入

        string best = "";

        for (int mask = 1; mask < (1<<9); ++mask) {
            array<int,10> cnt{};
            int L = 0;
            for (int d = 1; d <= 9; ++d) {
                if (mask & (1 << (d-1))) {
                    cnt[d] = d;
                    L += d;
                }
            }
            if (L == 0) continue;
            if (L < len_n) continue;

            int oddCnt = 0, oddDigit = -1;
            for (int d = 1; d <= 9; ++d) {
                if (cnt[d] % 2 == 1) {
                    oddCnt++;
                    oddDigit = d;
                }
            }
            if (oddCnt > 1) continue;

            string half;
            for (int d = 1; d <= 9; ++d) {
                int h = cnt[d] / 2;
                half.append(h, char('0' + d));
            }
            sort(half.begin(), half.end());
            string mid = (L % 2 == 1 ? string(1, char('0' + oddDigit)) : string());

            string rev_half = half;
            reverse(rev_half.begin(), rev_half.end());
            string pal_min = half + mid + rev_half;

            if (L > len_n) {
                if (betterStr(pal_min, best)) best = pal_min;
                continue;
            }

            // L == len_n 情况
            if (pal_min > nstr) {
                if (betterStr(pal_min, best)) best = pal_min;
                continue;
            }

            // 需要枚举 half 的后续排列；从当前 half（最小）开始
            // 剪枝：若已有 best 且 best 的长度 <= len_n 且 best <= nstr 则 impossible; but generally we can stop if current half minimal candidate >= best
            string half_s = half;
            bool found = false;
            // compute threshold for early skipping: we want pal > nstr.
            // iterate permutations in lex order, break when find pal > nstr or when pal >= best (no hope)
            do {
                // early prune: if best exists and same length, and half_s + mid + reverse(...) >= best, then further permutations (lexicographically larger) cannot beat best
                string cur_pal = half_s;
                string cur_rev = half_s;
                reverse(cur_rev.begin(), cur_rev.end());
                cur_pal += mid;
                cur_pal += cur_rev;

                if (!best.empty()) {
                    if (!betterStr(cur_pal, best) && !(cur_pal < best)) {
                        // cur_pal >= best, and later permutations >= cur_pal, so cannot improve best
                        break;
                    }
                }

                if (cur_pal.size() > nstr.size() || (cur_pal.size() == nstr.size() && cur_pal > nstr)) {
                    if (betterStr(cur_pal, best)) best = cur_pal;
                    found = true;
                    break;
                }
            } while (next_permutation(half_s.begin(), half_s.end()));

            (void)found;
        }

        if (best.empty()) return -1;
        long long ans = 0;
        for (char c : best) ans = ans * 10 + (c - '0');
        return ans;
    }
};

int main() {

    Solution sol;

    // 示例测试
    vector<long long> examples = {2, 33, 0, 9, 22, 121, 1234LL, 999999999999999LL};
    cout << "示例测试:\n";
    for (long long x : examples) {
        cout << "n = " << x << " -> " << sol.specialPalindrome(x) << "\n";
    }

    // 题目示例单独确认
    cout << "\n题目示例:\n";
    cout << "n=2 -> " << sol.specialPalindrome(2) << " (期望 22)\n";
    cout << "n=33 -> " << sol.specialPalindrome(33) << " (期望 212)\n";

    // 边界及随机测试（打印若干以便人工检查）
    cout << "\n随机/边界样例:\n";
    vector<long long> rnds = {1, 10, 11, 19, 20, 21, 99, 100, 99999999999999LL};
    for (long long r : rnds) {
        cout << "n = " << r << " -> " << sol.specialPalindrome(r) << "\n";
    }

    // 额外：穷举 small n，检查递增性（可选）
    /*
    for (int i = 0; i <= 5000; ++i) {
        long long v = sol.specialPalindrome(i);
        if (v <= i) {
            cerr << "错误: n="<<i<<" 返回 "<<v<<"\n";
            return 0;
        }
    }
    cerr << "小范围检查通过\n";
    */

    return 0;
}
