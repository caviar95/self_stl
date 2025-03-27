// ./a.out 3 "1.0.0.1 2.0.0.1 上海浦东" "2.0.0.2 3.0.0.1 湖北武汉" "3.0.0.3 4.1.1.1 深圳南山" 2.2.2.2

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Entry {
    unsigned int start;
    unsigned int end;
    string region;
};

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

    string queryIp = argv[2 + n];
    unsigned int query = ipToInt(queryIp);

    for (const auto& entry : entries) {
        if (query >= entry.start && query <= entry.end) {
            cout << entry.region << endl;

            auto end = high_resolution_clock::now();
            auto duration_original = duration_cast<microseconds>(end - start).count();

            cout << "cost: " << duration_original << endl;
            return 0;
        }
    }

    return 0;
}