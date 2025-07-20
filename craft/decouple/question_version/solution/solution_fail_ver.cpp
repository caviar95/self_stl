#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <cassert>

// ---- C++14 index_sequence ----
template <std::size_t... Is>
struct index_sequence {};

template <std::size_t N, std::size_t... Is>
struct make_index_sequence : make_index_sequence<N - 1, N - 1, Is...> {};

template <std::size_t... Is>
struct make_index_sequence<0, Is...> {
    using type = index_sequence<Is...>;
};

// ---- apply 实现 ----
template <typename Func, typename Tuple, std::size_t... Is>
auto apply_impl(Func&& f, Tuple&& t, index_sequence<Is...>)
    -> decltype(f(std::get<Is>(std::forward<Tuple>(t))...)) {
    return f(std::get<Is>(std::forward<Tuple>(t))...);
}

template <typename Func, typename Tuple>
auto apply(Func&& f, Tuple&& t)
    -> decltype(apply_impl(
        std::forward<Func>(f),
        std::forward<Tuple>(t),
        typename make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>::type{})) {
    return apply_impl(std::forward<Func>(f), std::forward<Tuple>(t),
                      typename make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>::type{});
}

// ---- vector<string> -> tuple<string, string, ...> ----
template <size_t... I>
auto VectorToTupleImpl(const std::vector<std::string>& vec, index_sequence<I...>) {
    return std::make_tuple((I < vec.size() ? vec[I] : std::string{})...);
}

template <size_t N>
auto VectorToTuple(const std::vector<std::string>& vec) {
    return VectorToTupleImpl(vec, typename make_index_sequence<N>::type{});
}

// ---- Platform-level Report ----
template <typename... Args>
void Report(bool mode, Args&&... args) {
    if (mode) return;
    std::cout << "Report: ";
    int dummy[] = { (std::cout << args << " ", 0)... };
    (void)dummy;
    std::cout << std::endl;
}

// ---- New safe entry point ----
template <size_t MaxArgs = 5>
void ReportFromVectorSafe(bool mode, const std::vector<std::string>& vec) {
    assert(vec.size() <= MaxArgs); // runtime check
    auto tup = VectorToTuple<MaxArgs>(vec);
    apply([&](const std::string&... unpacked) {
        Report(mode, unpacked...);
    }, tup);
}

// ---- Main test ----
int main() {
    std::vector<std::string> vec = {"disk C", "95%", "full"};
    ReportFromVectorSafe<5>(false, vec);  // ✅ Pass
    return 0;
}
