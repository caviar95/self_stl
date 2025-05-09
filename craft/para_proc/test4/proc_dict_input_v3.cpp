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

// 二分查找核心逻辑：直接定位目标区间
const Entry* findRegion(const vector<Entry>& entries, unsigned int query) {
    int left = 0, right = entries.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (query < entries[mid].start) {
            right = mid - 1;
        } else if (query > entries[mid].end) {
            left = mid + 1;
        } else {
            // 找到覆盖目标IP的区间
            return &entries[mid];
        }
    }
    
    // 未找到时，检查可能的左邻居（因区间可能重叠）
    if (right >= 0 && query <= entries[right].end) {
        return &entries[right];
    }
    return nullptr;
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

    string queryIp = argv[2 + n];
    unsigned int query = ipToInt(queryIp);

    // 二分查找目标区间
    const Entry* result = findRegion(entries, query);
    if (result) {
        cout << result->region << endl;
    } else {
        cout << "No matching region found." << endl;
    }
    auto end = high_resolution_clock::now();
    auto duration_original = duration_cast<microseconds>(end - start).count();

    cout << "cost: " << duration_original << endl;
    return 0;
}