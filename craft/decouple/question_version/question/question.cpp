#include <iostream>
#include <string>
#include <vector>

template <typename... Args>
void Report(bool mode, Args&&... args) {
    std::cout << "Reporting: ";
    int dummy[] = { (std::cout << args << " ", 0)... };
    (void)dummy;
    std::cout << "\n";
}

template <typename... Args>
void ReportFromVector(bool mode, const std::vector<std::string>& vec, size_t index, Args&&... args) {
    if (index == vec.size()) {
        Report(mode, std::forward<Args>(args)...);
    } else {
        // 🧨 每递归一次，Args... 增长，实例化新模板
        ReportFromVector(mode, vec, index + 1, std::forward<Args>(args)..., vec[index]);
    }
}

int main() {
    std::vector<std::string> v(1000, "test"); // 模拟很多参数
    ReportFromVector(false, v, 0);
    return 0;
}
