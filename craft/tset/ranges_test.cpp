#include <ranges>
#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <string>

// void test() {
//     std::vector<int> v(3);

//     std::ranges::iota(v, 0);
//     // std::iota(v.begin(), v.end(), 0);

//     for (auto i : v) {
//         std::cout << i << " ";
//     }
// }


void test_2() {
    std::vector<std::string> words{"a","b","c","ab","bc","abc"};

    std::string s{"abc"};

    std::cout << std::ranges::count_if(words, [&](std::string &word) {return s.starts_with(word);}) << std::endl;
    // std::cout << std::count_if(words.begin(), words.end(), [&](std::string &word) {return s.starts_with(word);}) << std::endl;
}

int main() {
    // test();

    test_2();
    return 0;
}