#include <iostream>
#include <vector>

using namespace std;

unsigned long long catalan(int n) {
    vector<unsigned long long> dp(n + 1);

    dp[0] = dp[1] = 1;

    for (int i = 2; i <= n; ++i) {
        for (int j = 0; j < i; ++j) {
            dp[i] += dp[j] * dp[i - 1 - j];
        }
    }

    return dp[n];
}

int main() {
    int n;
    cout << "node number: ";
    cin >> n;

    cout << "result: " << catalan(n) << endl;

    return 0;
}
