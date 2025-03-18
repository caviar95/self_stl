#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

string commonPrefix(const string& str1, const string& str2) {
    int minLength = min(str1.length(), str2.length());
    for (int i = 0; i < minLength; ++i) {
        if (str1[i] != str2[i]) {
            return str1.substr(0, i);
        }
    }
    return str1.substr(0, minLength);
}

int longestCommonPrefixLength(const string& str1, const string& str2) {
    int i = 0;
    while (i < str1.length() && i < str2.length() && str1[i] == str2[i]) {
        ++i;
    }
    return i;
}

int findLongestCommonPrefixLength(const vector<string>& words, int start, int k) {
    if (k == 0) return 0;
    string prefix = words[start];
    for (int i = 1; i < k; ++i) {
        prefix = commonPrefix(prefix, words[start + i]);
        if (prefix.empty()) return 0;
    }
    return prefix.length();
}




vector<int> longestCommonPrefix(vector<string>& words, int k) {
    int n = words.size();
    vector<int> result;
    
    for (int i = 0; i < n; ++i) {
        // Skip the current word and find the longest k-1 strings (or less) in the remaining array
        vector<string> remainingWords(words.begin(), words.begin() + i);
        remainingWords.insert(remainingWords.end(), words.begin() + i + 1, words.end());
        
        // If there are fewer than k words left, add zeros to the result
        if (remainingWords.size() < k) {
            result.push_back(0);
            continue;
        }
        
        // Sort the remaining words by length (descending) to potentially reduce the number of comparisons
        sort(remainingWords.begin(), remainingWords.end(), [](const string& a, const string& b) {
            return a.length() > b.length();
        });
        
        // Find the length of the longest common prefix among the top k strings
        int longestPrefixLen = 0;
        string currentPrefix = remainingWords[0];
        for (int j = 0; j < k; ++j) {
            longestPrefixLen = longestCommonPrefixLength(currentPrefix, remainingWords[j]);
            if (longestPrefixLen == 0) break; // No need to continue if prefix is already zero
        }
        
        result.push_back(longestPrefixLen);
    }
    
    return result;
}

vector<int> longestCommonPrefixOptimized(vector<string>& words, int k) {
    int n = words.size();
    vector<int> result;
    
    auto lcp = [&](const vector<string>& subset) -> int {
        if (subset.empty()) return 0;
        string prefix = subset[0];
        for (size_t i = 1; i < subset.size(); ++i) {
            int len = longestCommonPrefixLength(prefix, subset[i]);
            if (len == 0) return 0;
            prefix = prefix.substr(0, len);
            if (i == subset.size() - 1 || prefix.empty()) break; // Early exit if done or prefix becomes empty
        }
        return prefix.length();
    };
    
    for (int i = 0; i < n; ++i) {
        vector<string> remainingWords;
        for (int j = 0; j < n; ++j) {
            if (j != i) remainingWords.push_back(words[j]);
        }
        
        if (remainingWords.size() < k) {
            result.push_back(0);
        } else {
            sort(remainingWords.begin(), remainingWords.end(), [](const string& a, const string& b) {
                return a.length() > b.length();
            });
            
            result.push_back(lcp(vector<string>(remainingWords.begin(), remainingWords.begin() + k)));
        }
    }
    
    return result;
}


int main() {
    vector<string> words = {"jump", "run", "run", "jump", "run"};
    int k = 2;
    vector<int> result = longestCommonPrefixOptimized(words, k);
    for (int len : result) {
        cout << len << " ";
    }
    cout << endl;
    return 0;
}