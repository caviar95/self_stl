#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <cassert>

// ========== [1] 模拟 source_location ==========
struct source_location {
    std::string file;
    int line;

    static source_location current(const char* file = __FILE__, int line = __LINE__) {
        return {file, line};
    }

    std::string str() const {
        return file + ":" + std::to_string(line);
    }
};

// ========== [2] 模拟平台接口 ==========
template <typename... Args>
int Report(bool mode, source_location loc, Args&&... args) {
    if (mode) return 0;
    std::cerr << "[Report] At " << loc.str() << ": ";
    int dummy[] = {(std::cerr << args << " ", 0)...};
    (void)dummy;
    std::cerr << std::endl;
    return -1;
}

// ========== [3] 错误码格式映射 ==========
std::unordered_map<std::string, std::string> g_exceptionMessageTemplates = {
    {"E1001", "Disk {0} is full, used {1}%"},
    {"E2001", "User {0} not found"},
    {"E3001", "File {0} cannot be opened: {1}"},
};

// ========== [4] 简单格式化器 ==========
std::string FormatMessage(const std::string& code, const std::vector<std::string>& args) {
    auto it = g_exceptionMessageTemplates.find(code);
    if (it == g_exceptionMessageTemplates.end()) return "[Unknown code]";

    std::string format = it->second;
    for (size_t i = 0; i < args.size(); ++i) {
        std::string token = "{" + std::to_string(i) + "}";
        size_t pos = format.find(token);
        while (pos != std::string::npos) {
            format.replace(pos, token.length(), args[i]);
            pos = format.find(token, pos + args[i].length());
        }
    }
    return format;
}

// ========== [5] apply() for C++14 ==========
template <typename Func, typename Tuple, std::size_t... Is>
auto apply_impl(Func&& f, Tuple&& t, std::index_sequence<Is...>) {
    return f(std::get<Is>(t)...);
}

template <typename Func, typename Tuple>
auto apply(Func&& f, Tuple&& t) {
    constexpr std::size_t N = std::tuple_size<typename std::decay<Tuple>::type>::value;
    return apply_impl(std::forward<Func>(f), std::forward<Tuple>(t), std::make_index_sequence<N>{});
}

// ========== [6] vector<string> → tuple<string...> ==========
template <std::size_t... Is>
auto VectorToTupleImpl(const std::vector<std::string>& vec, std::index_sequence<Is...>) {
    return std::make_tuple(vec[Is]...);
}

template <std::size_t N>
auto VectorToTuple(const std::vector<std::string>& vec) {
    assert(vec.size() == N);
    return VectorToTupleImpl(vec, std::make_index_sequence<N>{});
}

// ========== [7] Report bridge ==========
void ReportException(bool mode, const std::string& code, const std::vector<std::string>& args, source_location loc = source_location::current()) {
    std::string message = FormatMessage(code, args);

    switch (args.size()) {
        case 0:
            Report(mode, loc, message);
            break;
        case 1:
            apply([&](const std::string& a1) {
                Report(mode, loc, message, a1);
            }, VectorToTuple<1>(args));
            break;
        case 2:
            apply([&](const std::string& a1, const std::string& a2) {
                Report(mode, loc, message, a1, a2);
            }, VectorToTuple<2>(args));
            break;
        case 3:
            apply([&](const std::string& a1, const std::string& a2, const std::string& a3) {
                Report(mode, loc, message, a1, a2, a3);
            }, VectorToTuple<3>(args));
            break;
        case 4:
            apply([&](const std::string& a1, const std::string& a2, const std::string& a3, const std::string& a4) {
                Report(mode, loc, message, a1, a2, a3, a4);
            }, VectorToTuple<4>(args));
            break;
        case 5:
            apply([&](const std::string& a1, const std::string& a2, const std::string& a3, const std::string& a4, const std::string& a5) {
                Report(mode, loc, message, a1, a2, a3, a4, a5);
            }, VectorToTuple<5>(args));
            break;
        default:
            std::cerr << "[ERROR] Too many arguments: " << args.size() << std::endl;
            break;
    }
}

// ========== [8] 测试 ==========
int main() {
    ReportException(false, "E1001", {"C", "95"});      // Disk C is full, used 95%
    ReportException(false, "E2001", {"admin"});        // User admin not found
    ReportException(false, "E3001", {"config.yaml", "Permission denied"});
    ReportException(false, "E9999", {"a", "b"});        // Unknown code
}
