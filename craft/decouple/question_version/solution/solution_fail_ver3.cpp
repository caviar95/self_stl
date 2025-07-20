#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <cassert>

// ---- index_sequence / make_index_sequence ----
template <std::size_t... Is>
struct index_sequence {};

template <std::size_t N, std::size_t... Is>
struct make_index_sequence : make_index_sequence<N - 1, N - 1, Is...> {};

template <std::size_t... Is>
struct make_index_sequence<0, Is...> {
    using type = index_sequence<Is...>;
};

// ---- apply-like: tuple → unpack → Report ----
template <typename Tuple, std::size_t... Is>
void ReportFromTuple(bool mode, Tuple&& tup, index_sequence<Is...>) {
    Report(mode, std::get<Is>(std::forward<Tuple>(tup))...);
}

template <typename Tuple>
void ReportFromTuple(bool mode, Tuple&& tup) {
    constexpr std::size_t N = std::tuple_size<typename std::decay<Tuple>::type>::value;
    ReportFromTuple(mode, std::forward<Tuple>(tup), typename make_index_sequence<N>::type{});
}

// ---- Convert vector<string> to tuple<string, ..., N> ----
template <std::size_t... I>
auto VectorToTupleImpl(const std::vector<std::string>& vec, index_sequence<I...>) {
    return std::make_tuple((I < vec.size() ? vec[I] : std::string{})...);
}

template <std::size_t N>
auto VectorToTuple(const std::vector<std::string>& vec) {
    return VectorToTupleImpl(vec, typename make_index_sequence<N>::type{});
}

// ---- Your Report function (platform-level) ----
template <typename... Args>
void Report(bool mode, Args&&... args) {
    if (mode) return;
    std::cout << "Report: ";
    int dummy[] = { (std::cout << args << " ", 0)... };
    (void)dummy;
    std::cout << std::endl;
}

// ---- Safe wrapper to report vector<string> as multiple args ----
template <std::size_t MaxArgs = 5>
void ReportFromVectorSafe(bool mode, const std::vector<std::string>& vec) {
    assert(vec.size() <= MaxArgs);
    auto tup = VectorToTuple<MaxArgs>(vec);
    ReportFromTuple(mode, tup);
}

// ---- Main: test ----
int main() {
    std::vector<std::string> vec1 = {"disk", "C"};
    ReportFromVectorSafe<5>(false, vec1);

    std::vector<std::string> vec2 = {"user", "1234", "not", "found"};
    ReportFromVectorSafe<5>(false, vec2);

    std::vector<std::string> vec3 = {"Error"};
    ReportFromVectorSafe<5>(false, vec3);

    return 0;
}
