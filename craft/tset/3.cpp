vector<int> longestCommonPrefix(vector<string>& words, int k) {
    int n = words.size();
    vector<int> res(n, 0);


    vector<int> sorted_indices(n);
    iota(sorted_indices.begin(), sorted_indices.end(), 0);
    sort(sorted_indices.begin(), sorted_indices.end(), 
        [&words](int a, int b) { return words[a] < words[b]; });

    for (int i = 0; i < n; ++i) {
        vector<int> window_indices;
        window_indices.reserve(n - 1);
        for (int idx : sorted_indices) {
            if (idx != i) window_indices.push_back(idx);
        }
        
        int m = window_indices.size();
        if (m < k) {
            res[i] = 0;
            continue;
        }
        
        int max_len = 0;
        for (int j = 0; j <= m - k; ++j) {
            const string& s1 = words[window_indices[j]];
            const string& s2 = words[window_indices[j + k - 1]];
            int lcp = 0;
            int min_len = min(s1.size(), s2.size());
            while (lcp < min_len && s1[lcp] == s2[lcp]) {
                lcp++;
                if (lcp <= max_len) break;  // 提前终止
            }
            if (lcp > max_len) max_len = lcp;
        }
        res[i] = max_len;
    }
    return res;
}
