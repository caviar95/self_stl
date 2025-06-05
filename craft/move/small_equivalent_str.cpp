class Solution {
public:
    string smallEquivalString(string s1, string s2, string baseStr) {
        vector<int> parent(26);

        iota(parent.begin(), parent.end(), 0);

        auto find = [&](this auto&& find, int x) {
            if (parent[x] != x) {
                parent[x] = find(parent[x]):
            }

            return parent[x];
        };

        auto unite = [&](int a, int b) -> void {
            int pa = find(a);
            int pb = find(b);

            if (pa != pb) {
                parent[pb] = pa;
            } else {
                parent[pa] = pb;
            }
        };

        for (int i = 0; i < s1.length(); ++i) {
            unite(s1[i] - 'a', s2[i] - 'a');
        }

        string ans;
        for (char c : baseStr) {
            ans += static_cast<char>(find(c - 'a') + 'a');
        }

        return ans;
    }
};
