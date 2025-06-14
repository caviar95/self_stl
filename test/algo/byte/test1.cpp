#include <vector>
#include <iostream>
#include <climits>
using namespace std;

class Solution {
public:
    bool canMakeEqual(vector<int>& nums, int k) {
        int n = nums.size();
        int sign = 1;
        for (int x : nums) {
            sign *= x;
        }

        if (sign == -1 && n % 2 == 0) {
            return false;
        }

        vector<int> flip1;
        vector<int> flip2;

        for (int i = 0; i < n; i++) {
            if (nums[i] == -1) {
                flip1.emplace_back(i);
            } else {
                flip2.emplace_back(i);
            }
        }

        auto computeCost = [](vector<int>& flip) -> long long {
            if (flip.size() % 2 != 0) {
                return LLONG_MAX;
            }

            long long cost = 0;

            for (int i = 0; i < flip.size(); i += 2) {
                cost += static_cast<long long>(flip[i+1] - flip[i]);
            }

            return cost;
        };

        if (n % 2 == 1) {
            if (sign == 1) {
                long long cost = computeCost(flip1);
                return cost <= k;
            }
            long long cost = computeCost(flip2);
            return cost <= k;

        }

        if (sign == -1) {
            return false;
        }
        
        return (computeCost(flip1) <= k) || (computeCost(flip2) <= k);
    }
};


int main() {
    vector<int> arr{1,-1,1,-1,1};

    cout << Solution{}.canMakeEqual(arr, 3) << endl;

    return 0;

}