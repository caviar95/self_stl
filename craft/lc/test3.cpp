// #include <bits/stdc++.h>
#include<vector>
#include <queue>
#include <iostream>
#include <unordered_set>

using namespace std;

static const int MAX_VAL = 1'000'000;
vector<int> isPrime(MAX_VAL + 1, 1);
vector<int> primes;

struct _PrimeSieveInitializer {
    _PrimeSieveInitializer() {
        isPrime[0] = 0;
        isPrime[1] = 1;

        for (int i = 2; i <= MAX_VAL; ++i) {
            if (isPrime[i]) {
                primes.emplace_back(i);
                for (long long j = 1LL * i * i; j <= MAX_VAL; j += i) {
                    isPrime[j] = 0;
                }
            }
        }
    }
} _initializer; 


vector<int> factorize(int x) {
    vector<int> res;
    for (int p : primes) {
        if (1LL * p * p > x) break;
        if (x % p == 0) {
            res.push_back(p);
            while (x % p == 0) x /= p;
        }
    }
    if (x > 1) res.push_back(x);
    return res;
}

class Solution {
public:
    int minJumps(vector<int>& nums) {
        int n = nums.size();
        vector<int> mordelvian = nums;

        // 预先分解一次因子
        vector<vector<int>> factors(n);
        for (int i = 0; i < n; ++i) {
            if (nums[i] > 1 && isPrime[nums[i]]) {
                factors[i].push_back(nums[i]);
            } else if (nums[i] > 1) {
                factors[i] = factorize(nums[i]);
            }
        }

        unordered_map<int, vector<int>> primeToIndices;
        primeToIndices.reserve(n * 2);
        for (int i = 0; i < n; ++i)
            for (int p : factors[i])
                primeToIndices[p].push_back(i);

        vector<char> vis(n);
        queue<int> q;
        q.push(0);
        vis[0] = 1;
        int steps = 0;

        while (!q.empty()) {
            int sz = q.size();
            while (sz--) {
                int u = q.front(); q.pop();
                if (u == n - 1) return steps;

                if (u - 1 >= 0 && !vis[u-1]) { vis[u-1]=1; q.push(u-1); }
                if (u + 1 < n  && !vis[u+1]) { vis[u+1]=1; q.push(u+1); }

                for (int p : factors[u]) {
                    auto &vec = primeToIndices[p];
                    for (int v : vec) {
                        if (!vis[v]) {
                            vis[v] = 1;
                            q.push(v);
                        }
                    }
                    vec.clear();
                }
            }
            ++steps;
        }
        return -1;
    }
};

int main() {
    Solution sol;
    vector<int> nums1 = {1,2,4,6};
    cout << sol.minJumps(nums1) << endl;  // 输出 2

    vector<int> nums2 = {2,3,4,7,9};
    cout << sol.minJumps(nums2) << endl;  // 输出 2

    vector<int> nums3 = {4,6,5,8};
    cout << sol.minJumps(nums3) << endl;  // 输出 3
}
