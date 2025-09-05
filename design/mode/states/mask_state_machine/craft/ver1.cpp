#include <iostream>
#include <tuple>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <functional>
#include <mutex>
#include <shared_mutex>

// ---- 通用宏：枚举及字符串映射 ----
#define DECLARE_ENUM_WITH_STRINGS(NAME, LIST)                      \
    enum class NAME { LIST(ENUM_ITEM, NAME) };                     \
    inline const char* to_string(NAME v) {                         \
        switch (v) { LIST(ENUM_CASE, NAME) }                       \
        return "Unknown";                                          \
    }

#define ENUM_ITEM(x, NAME) x,
#define ENUM_CASE(x, NAME) case NAME::x: return #x;

// ---- 定义各子系统状态 ----
#define ENGINE_STATES(M, N)   M(Idle, N) M(Running, N) M(Failed, N)
#define NETWORK_STATES(M, N)  M(Disconnected, N) M(Connected, N) M(Error, N)
#define POWER_STATES(M, N)    M(Off, N) M(On, N) M(Overload, N)
#define SENSOR_STATES(M, N)   M(Cold, N) M(Warm, N) M(Hot, N)

DECLARE_ENUM_WITH_STRINGS(EngineState, ENGINE_STATES)
DECLARE_ENUM_WITH_STRINGS(NetworkState, NETWORK_STATES)
DECLARE_ENUM_WITH_STRINGS(PowerState, POWER_STATES)
DECLARE_ENUM_WITH_STRINGS(SensorState, SENSOR_STATES)

// ---- 通用工具：集合打印 ----
template<typename T>
std::string set_to_string(const std::set<T>& s) {
    std::ostringstream oss;
    bool first = true;
    for (auto& e : s) {
        if (!first) oss << '|';
        oss << to_string(e);
        first = false;
    }
    return oss.str();
}

// ---- 子系统匹配策略 ----
template<typename EnumT>
struct SubMatcher {
    std::set<EnumT> accepted;
    std::function<bool(const EnumT&)> custom_match; // 可选策略

    bool matches(const EnumT& value) const {
        if (custom_match) return custom_match(value);
        return accepted.count(value) > 0;
    }

    std::string to_string() const {
        return set_to_string(accepted);
    }
};

// ---- 组合状态 ----
template<typename... Subsystems>
class CombinedState {
    std::tuple<Subsystems...> subs;
public:
    CombinedState(Subsystems... s) : subs(s...) {}

    template<std::size_t I>
    auto get() const { return std::get<I>(subs); }

    std::string to_string() const {
        std::ostringstream oss;
        bool first = true;
        to_string_impl(oss, first, std::index_sequence_for<Subsystems...>{});
        return oss.str();
    }

private:
    template<std::size_t... I>
    void to_string_impl(std::ostringstream &oss, bool &first, std::index_sequence<I...>) const {
        (([&] {
            if (!first) oss << ", ";
            first = false;
            oss << "sub" << I << ":" << ::to_string(std::get<I>(subs));
        }()), ...);
    }
};

// ---- 最终状态描述 ----
template<typename... Subsystems>
class FinalState {
    std::tuple<SubMatcher<Subsystems>...> mask;
    std::string name;
public:
    FinalState(std::string n, std::tuple<SubMatcher<Subsystems>...> m)
        : mask(std::move(m)), name(std::move(n)) {}

    bool matches(const CombinedState<Subsystems...>& st) const {
        return matches_impl(st, std::index_sequence_for<Subsystems...>{});
    }

    std::string to_string() const {
        std::ostringstream oss;
        bool first = true;
        mask_to_string_impl(oss, first, std::index_sequence_for<Subsystems...>{});
        return name + " {" + oss.str() + "}";
    }

private:
    template<std::size_t... I>
    bool matches_impl(const CombinedState<Subsystems...>& st, std::index_sequence<I...>) const {
        return (... && std::get<I>(mask).matches(st.template get<I>()));
    }

    template<std::size_t... I>
    void mask_to_string_impl(std::ostringstream& oss, bool& first, std::index_sequence<I...>) const {
        (([&] {
            if (!first) oss << ", ";
            first = false;
            oss << "sub" << I << ":" << std::get<I>(mask).to_string();
        }()), ...);
    }
};

// ---- 状态机基类 ----
template<typename... Subsystems>
class StateMachine {
    using Final = FinalState<Subsystems...>;
    std::vector<Final> finals;
    mutable std::shared_mutex mtx;

public:
    void register_final(Final f) {
        std::unique_lock lock(mtx);
        finals.emplace_back(std::move(f));
    }

    void unregister_all() {
        std::unique_lock lock(mtx);
        finals.clear();
    }

    std::vector<std::string> match(const CombinedState<Subsystems...>& st) const {
        std::shared_lock lock(mtx);
        std::vector<std::string> matched;
        for (auto& f : finals) {
            if (f.matches(st)) matched.push_back(f.to_string());
        }
        return matched;
    }
};

// ---- 示例用法 ----
int main() {
    using MyMachine = StateMachine<EngineState, NetworkState, PowerState, SensorState>;

    MyMachine sm;

    // 定义一个最终状态
    SubMatcher<EngineState> eng;
    eng.accepted = {EngineState::Idle, EngineState::Running};

    SubMatcher<NetworkState> net;
    net.accepted = {NetworkState::Connected};

    SubMatcher<PowerState> power;
    power.accepted = {PowerState::On};

    SubMatcher<SensorState> sensor;
    sensor.custom_match = [](SensorState s) { return s != SensorState::Hot; };

    sm.register_final(FinalState<EngineState, NetworkState, PowerState, SensorState>(
        "SafeState",
        std::make_tuple(eng, net, power, sensor)
    ));

    CombinedState<EngineState, NetworkState, PowerState, SensorState> cs(
        EngineState::Running, NetworkState::Connected, PowerState::On, SensorState::Warm
    );

    for (auto& r : sm.match(cs)) {
        std::cout << "Matched: " << r << "\n";
    }

    return 0;
}
