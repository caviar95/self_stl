#include <iostream>
#include <fstream>
#include <string>

int CountOccurrences(const std::string& text, const std::string& target) {
    if (target.empty()) {
        return 0;
    }

    int count{};
    for (size_t pos = 0; (text.find(target, pos)) != std::string::npos; ++pos) {
        ++count;
    }

    return count;
}

int main() {
    const std::string filename = __FILE__;
    const std::string target = "int";

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "cannot open file: " << filename << std::endl;
        return 1;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    int occurrences = CountOccurrences(content, target);

    std::cout << "string \"" << target << "\" occurs " << occurrences << " times." << std::endl;

    return 0;

}