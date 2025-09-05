#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <bitset>
#include <shared_mutex>
#include <optional>
#include <algorithm>

// ========== X-Macro 定义与工具 ==========
#define DECLARE_ENUM_WITH_STRINGS(NAME, LIST) \
    enum class NAME { LIST(ENUM_ITEM, NAME) }; \
    inline const char* to_string(NAME v) { \
        switch(v) { LIST(ENUM_CASE, NAME) } \
        return "Unknown"; \
    }

#define ENUM_ITEM(x, NAME) x,
#define ENUM_CASE(x, NAME) case NAME::x: return #x;

#define ENGINE_STABLE(M, N) M(Idle, N) M(Running, N) M(Failed, N)
#define ENGINE_TEMP(M, N)   M(HeatingUp, N) M(CoolingDown, N)

#define NETWORK_STABLE(M, N) M(Disconnected, N) M(Connected, N)
#define NETWORK_TEMP(M, N)   M(Reconnecting, N)

#define POWER_STABLE(M, N)   M(Off, N) M(On, N)
#define POWER_TEMP(M, N)     M(Overload, N)

#define SENSOR_STABLE(M, N)  M(Normal, N) M(Warning, N) M(Critical, N)
#define SENSOR_TEMP(M, N)    M(Calibrating, N)

DECLARE_ENUM_WITH_STRINGS(EngineStable, ENGINE_STABLE)
DECLARE_ENUM_WITH_STRINGS(EngineTemp, ENGINE_TEMP)
DECLARE_ENUM_WITH_STRINGS(NetworkStable, NETWORK_STABLE)
DECLARE_ENUM_WITH_STRINGS(NetworkTemp, NETWORK_TEMP)
DECLARE_ENUM_WITH_STRINGS(PowerStable, POWER_STABLE)
DECLARE_ENUM_WITH_STRINGS(PowerTemp, POWER_TEMP)
DECLARE_ENUM_WITH_STRINGS(SensorStable, SENSOR_STABLE)
DECLARE_ENUM_WITH_STRINGS(SensorTemp, SENSOR_TEMP)

// ========== 掩码定义 ==========
template<typename EnumT>
struct EnumMask {
    using underlying = uint64_t;
    static underlying bit(EnumT e) {
        return 1ULL << static_cast<int>(e);
    }
};

// 每个子系统用一个结构表示 stable/temp 掩码
template<typename StableT, typename TempT>
struct SubsystemMask {
    uint64_t stable_mask{0};
    uint64_t temp_mask{0};

    void set(StableT s) { stable_mask |= EnumMask<StableT>::bit(s); }
    void set(TempT t)   { temp_mask |= EnumMask<TempT>::bit(t); }
    void reset() { stable_mask = 0; temp_mask = 0; }
};

// 系统整体状态
struct CombinedMask {
    SubsystemMask<EngineStable, EngineTemp> engine;
    SubsystemMask<NetworkStable, NetworkTemp> network;
    SubsystemMask<PowerStable, PowerTemp> power;
    SubsystemMask<SensorStable, SensorTemp> sensor;
};

// FinalState 描述
struct FinalState {
    std::string name;
    CombinedMask required;   // 必须包含的掩码
    CombinedMask forbidden;  // 不能包含的掩码（除 except 用）
    int priority{0};

    // except 集合：一些特殊禁止组合
    std::vector<CombinedMask> except;

    bool matches(const CombinedMask& cur) const {
        // forbidden 检查
        if ((cur.engine.stable_mask & forbidden.engine.stable_mask) ||
            (cur.network.stable_mask & forbidden.network.stable_mask) ||
            (cur.power.stable_mask & forbidden.power.stable_mask) ||
            (cur.sensor.stable_mask & forbidden.sensor.stable_mask)) {
            return false;
        }

        // required 检查
        if ((cur.engine.stable_mask & required.engine.stable_mask) != required.engine.stable_mask) return false;
        if ((cur.network.stable_mask & required.network.stable_mask) != required.network.stable_mask) return false;
        if ((cur.power.stable_mask & required.power.stable_mask) != required.power.stable_mask) return false;
        if ((cur.sensor.stable_mask & required.sensor.stable_mask) != required.sensor.stable_mask) return false;

        // except 检查
        for (auto& e : except) {
            if ((cur.engine.stable_mask & e.engine.stable_mask) == e.engine.stable_mask &&
                (cur.network.stable_mask & e.network.stable_mask) == e.network.stable_mask &&
                (cur.power.stable_mask & e.power.stable_mask) == e.power.stable_mask &&
                (cur.sensor.stable_mask & e.sensor.stable_mask) == e.sensor.stable_mask) {
                return false;
            }
        }
        return true;
    }
};

// FinalState 注册与查询
class FinalStateRegistry {
    std::vector<FinalState> finals;
    mutable std::shared_mutex mutex;

public:
    void register_final(const FinalState& fs) {
        std::unique_lock lock(mutex);
        finals.push_back(fs);
    }

    void unregister_all() {
        std::unique_lock lock(mutex);
        finals.clear();
    }

    // 返回最高优先级的匹配状态
    std::optional<FinalState> best_match(const CombinedMask& cur) const {
        std::shared_lock lock(mutex);
        std::optional<FinalState> best;
        for (auto& f : finals) {
            if (f.matches(cur)) {
                if (!best || f.priority > best->priority) {
                    best = f;
                }
            }
        }
        return best;
    }
};

// ========== 使用示例 ==========
int main() {
    FinalStateRegistry registry;

    // 定义一个FinalState: Engine Running + Network Connected -> "OPERATIONAL"
    FinalState operational;
    operational.name = "OPERATIONAL";
    operational.priority = 10;
    operational.required.engine.set(EngineStable::Running);
    operational.required.network.set(NetworkStable::Connected);

    registry.register_final(operational);

    // 当前状态
    CombinedMask cur;
    cur.engine.set(EngineStable::Running);
    cur.network.set(NetworkStable::Connected);
    cur.power.set(PowerStable::On);
    cur.sensor.set(SensorStable::Normal);

    auto best = registry.best_match(cur);
    if (best) {
        std::cout << "Best match: " << best->name << "\n";
    } else {
        std::cout << "No match\n";
    }
    return 0;
}
