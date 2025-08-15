#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <type_traits>

/**
 * 模板化的掩码/状态编码器：
 * - MaskT: 掩码类型（默认 uint32_t）
 * - DEVICES: 设备数量（默认 4）
 * - BITS_PER_DEVICE: 每台设备的位宽（默认 8）
 *
 * 注意：请确保 DEVICES * BITS_PER_DEVICE <= (sizeof(MaskT)*8)
 * 若需要更多状态（例如每设备 16 位），可把 MaskT 换为 uint64_t 并把 BITS_PER_DEVICE 设为 16。
 */
template <
    typename MaskT = uint32_t,
    size_t DEVICES = 4,
    size_t BITS_PER_DEVICE = 8
>
class StateMachine {
    static_assert(std::is_unsigned<MaskT>::value, "MaskT must be unsigned integral");
    static_assert(DEVICES > 0, "DEVICES must be > 0");
    static_assert(BITS_PER_DEVICE > 0, "BITS_PER_DEVICE must be > 0");
    static_assert(DEVICES * BITS_PER_DEVICE <= (sizeof(MaskT) * 8),
                  "Not enough bits in MaskT for DEVICES * BITS_PER_DEVICE");

public:
    using Mask = MaskT;
    using FinalState = std::string; // 你也可以换成枚举/整数

    // ===== 编解码工具 =====
    static constexpr Mask fieldMask() {
        return (BITS_PER_DEVICE >= (sizeof(Mask) * 8))
               ? ~Mask(0)
               : (Mask(1) << BITS_PER_DEVICE) - 1;
    }

    static constexpr size_t bitOffset(size_t deviceIndex) {
        return deviceIndex * BITS_PER_DEVICE;
    }

    // 从 mask 中取某台设备的数值状态（数值编码，范围 0..(2^BITS_PER_DEVICE - 1)）
    static constexpr uint32_t getDeviceState(Mask m, size_t deviceIndex) {
        return static_cast<uint32_t>((m >> bitOffset(deviceIndex)) & fieldMask());
    }

    // 设置某台设备的数值状态
    static constexpr Mask setDeviceState(Mask m, size_t deviceIndex, uint32_t stateVal) {
        Mask fm = fieldMask();
        Mask shift = static_cast<Mask>(bitOffset(deviceIndex));
        // 清空对应域
        m &= ~(fm << shift);
        // 写入新值（截断到位宽）
        m |= (static_cast<Mask>(stateVal) & fm) << shift;
        return m;
    }

    // 便捷：由设备状态数组构造 mask（按设备 0..DEVICES-1 顺序）
    static constexpr Mask makeMask(const std::array<uint32_t, DEVICES>& states) {
        Mask m = 0;
        for (size_t i = 0; i < DEVICES; ++i) {
            m = setDeviceState(m, i, states[i]);
        }
        return m;
    }

    // 便捷：把 mask 解码为设备状态数组
    static std::array<uint32_t, DEVICES> decodeMask(Mask m) {
        std::array<uint32_t, DEVICES> arr{};
        for (size_t i = 0; i < DEVICES; ++i) {
            arr[i] = getDeviceState(m, i);
        }
        return arr;
    }

    // ===== 状态机 API =====

    // 添加映射：mask -> finalState
    void addMapping(Mask key, const FinalState& finalState) {
        table_[key] = finalState;
    }

    // 查找：没有则返回 "UNKNOWN"
    FinalState findFinalState(Mask key) const {
        auto it = table_.find(key);
        return (it == table_.end()) ? FinalState("UNKNOWN") : it->second;
    }

    // 比较两个掩码是否完全一致
    static bool compareMasks(Mask a, Mask b) { return a == b; }

    // 判断是否允许从 from → to（任一设备不允许降级则整体不允许）
    static bool canTransitionNoDowngrade(Mask from, Mask to) {
        for (size_t i = 0; i < DEVICES; ++i) {
            uint32_t cur = getDeviceState(from, i);
            uint32_t nxt = getDeviceState(to, i);
            if (nxt < cur) return false; // 出现降级
        }
        return true;
    }

    // 计算“夹紧”后的下一掩码：逐设备取 max(from_i, desired_i)，从而禁止降级
    static Mask clampNoDowngrade(Mask from, Mask desired) {
        Mask result = 0;
        for (size_t i = 0; i < DEVICES; ++i) {
            uint32_t cur = getDeviceState(from, i);
            uint32_t want = getDeviceState(desired, i);
            uint32_t allow = (want < cur) ? cur : want;
            result = setDeviceState(result, i, allow);
        }
        return result;
    }

    // 根据“业务期望”获取下一掩码（含禁止降级逻辑），然后可用该掩码去查 finalState
    Mask getNextMask(Mask current, Mask desired) const {
        return clampNoDowngrade(current, desired);
    }

    // 打印状态表（辅助调试）
    void printStateTable() const {
        std::cout << "State Table (mask -> finalState):\n";
        for (const auto& kv : table_) {
            std::cout << "  0x" << std::hex << +kv.first << std::dec
                      << " -> " << kv.second << "  [";
            auto arr = decodeMask(kv.first);
            for (size_t i = 0; i < DEVICES; ++i) {
                std::cout << arr[i] << (i + 1 == DEVICES ? "" : ", ");
            }
            std::cout << "]\n";
        }
    }

private:
    std::unordered_map<Mask, FinalState> table_;
};

#endif // STATE_MACHINE_H
