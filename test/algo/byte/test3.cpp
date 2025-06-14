#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <set>

using namespace std;

class Solution {
public:
    static constexpr int MOD = 1000000007;

    int countPartitions(vector<int>& nums, int k) {
        int n = nums.size();
        
        vector<int> dp(n + 1);
        vector<int> pre(n + 2);
        dp[0] = 1;
        pre[1] = 1;

        int left{};
        multiset<int> w;

        for (int right = 0; right < n; ++right) {
            w.emplace(nums[right]);

            while (*w.rbegin() - *w.begin() > k) {
                w.erase(w.find(nums[left]));
                ++left;
            }


            // 区间 dp 累加：dp[right+1] = pre[right+1] - pre[left]
            dp[right + 1] = (pre[right + 1] - pre[left] + MOD) % MOD;

            // 更新前缀和
            pre[right + 2] = (pre[right + 1] + dp[right + 1]) % MOD;
        }

        return dp[n];
    }
};

int main() {


    vector<vector<int>> arr;
    arr.emplace_back(vector{9,4,1,3,7});
    arr.emplace_back(vector{3,3,4});

    vector<int> ks{4, 0};
    for (int i = 0; i < arr.size(); ++i) {
        cout << Solution{}.countPartitions(arr[i], ks[i]) << endl;
    }


    return 0;

}