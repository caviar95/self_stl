#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Entry {
    unsigned int start;
    unsigned int end;
    string region;
};

// 将IP字符串转换为32位无符号整数
unsigned int ipToInt(const string& ip) {
    unsigned int a, b, c, d;
    char dot;
    istringstream iss(ip);
    iss >> a >> dot >> b >> dot >> c >> dot >> d;
    return (a << 24) | (b << 16) | (c << 8) | d;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Insufficient arguments." << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    if (argc != n + 3) {
        cerr << "Invalid number of arguments." << endl;
        return 1;
    }

    auto start = high_resolution_clock::now();

    // 解析字典条目并按起始IP排序
    vector<Entry> entries;
    for (int i = 0; i < n; ++i) {
        string entryStr = argv[2 + i];
        istringstream iss(entryStr);
        string startIp, endIp, region;
        if (!(iss >> startIp >> endIp >> region)) {
            cerr << "Invalid dictionary entry format: " << entryStr << endl;
            return 1;
        }
        entries.push_back({ipToInt(startIp), ipToInt(endIp), region});
    }

    // 按起始IP排序（预处理）
    sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return a.start < b.start;
    });

    // 获取查询IP
    string queryIp = argv[2 + n];
    unsigned int query = ipToInt(queryIp);

    // 二分查找定位候选区间
    auto it = upper_bound(entries.begin(), entries.end(), query,
        [](unsigned int value, const Entry& entry) {
            return value < entry.start;
        });

    // 向前遍历检查所有可能的区间
    bool found = false;
    while (it != entries.begin()) {
        --it;
        if (query >= it->start && query <= it->end) {
            cout << it->region << endl;
            auto end = high_resolution_clock::now();
            auto duration_original = duration_cast<microseconds>(end - start).count();
        
            cout << "cost: " << duration_original << endl;
            found = true;
            break;
        }
        if (it->start < query - (1 << 24)) break; // 优化：若起始IP远小于查询值，提前终止
    }

    if (!found) {
        cout << "No matching region found." << endl;
    }


    return 0;
}