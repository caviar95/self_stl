#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <map>
#include <set>

using namespace std;

// class Solution {
// public:
//     bool canMakeEqual(vector<int>& nums, int k) {
//         int n = nums.size();
//         vector<int> flip(n, 0); // 标记每个位置是否受到了操作的取反影响
//         int currFlip = 0; // 当前累计的取反影响次数
//         int operations = 0;

//         for (int i = 0; i < n - 1; ++i) {
//             int realValue = nums[i] * ((currFlip % 2 == 0) ? 1 : -1);
//             if (realValue != nums[0]) {
//                 if (operations >= k) return false;
//                 operations++;
//                 flip[i]++;
//                 currFlip++;
//             }
//             currFlip -= flip[i];
//         }

//         int lastValue = nums[n - 1] * ((currFlip % 2 == 0) ? 1 : -1);
//         return lastValue == nums[0];
//     }
// };


#include <algorithm>


class Solution {
public:
    static constexpr int MOD = 1000000007;
    int countPermutations(vector<int>& complexity) {
        int n = complexity.size();
        // 如果只有一台机器，只有 [0] 一种解锁顺序
        if (n <= 1) return 1;
        // 检查是否存在 i>0 使得 complexity[i] <= complexity[0]
        for (int i = 1; i < n; i++) {
            if (complexity[i] <= complexity[0]) {
                return 0;
            }
        }
        // 计算 (n-1)! % MOD
        long long ans = 1;
        for (int i = 1; i <= n-1; i++) {
            ans = ans * i % MOD;
        }
        return int(ans);
    }
};







int main() {
    // vector<vector<int>> arr;
    // arr.emplace_back(vector{1, -1, 1, -1, 1});


    // vector<int> ks{3};
    
    // for (int i = 0; i < ks.size(); ++i) {
    //     std::cout << Solution{}.canMakeEqual(arr[i], ks[i]) << std::endl;

    // }

    vector<vector<int>> arr;
    arr.emplace_back(vector{1, 2, 3});
    arr.emplace_back(vector{3,3,3,4,4,4});
    arr.emplace_back(vector{2,68,61});

    for (auto &a : arr) {
        cout << Solution{}.countPermutations(a) << endl;
    }


    return 0;

}
