#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <set>


using namespace std;

class Solution {
public:
    long long maxTotal(vector<int>& value, vector<int>& limit) {
        int n = value.size();

        vector<priority_queue<int>> buckets(n + 1);
        for (int i = 0; i < n; ++i) {
            buckets[limit[i]].push(value[i]);
        }

        set<int> avail;
        for (int l = 1; l <= n; ++l) if (!buckets[l].empty()) {
            avail.insert(l);
        }

        long long total = 0;
        int active_count = 0;
        priority_queue<int, vector<int>, greater<int>> active_limits;

        while (true) {
            auto it = avail.upper_bound(active_count);
            if (it == avail.end()) break;
            int L = *it;
            int val = buckets[L].top(); buckets[L].pop();
            total += val;
            active_limits.push(L);
            active_count += 1;
            if (buckets[L].empty()) {
                avail.erase(L);
            }
            int temp_x = active_count;
            while (!avail.empty() && *avail.begin() <= temp_x) {
                avail.erase(avail.begin());
            }
            
            while (!active_limits.empty() && active_limits.top() <= temp_x) {
                active_limits.pop();
                active_count -= 1;
            }
        }

        return total;
    }
};



int main() {

    vector<vector<int>> vals{
        {3, 5, 8},
        {4, 2, 6},
        {4, 1, 5, 2}
    };

    vector<vector<int>> lims {
        {2, 1, 3},
        {1, 1, 1},
        {3, 3, 2, 3}
    };

    for (auto i = 0; i < vals.size(); ++i) {
        cout << Solution{}.maxTotal(vals[i], lims[i]) << endl;
    }

    return 0;
}