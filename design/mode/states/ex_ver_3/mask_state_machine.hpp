// mask_state_machine.hpp
#pragma once

#include <cstdint>
#include <atomic>
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <type_traits>

namespace msf {

// 支持的 MaskType 必须是无符号整型
template<typename MaskType = uint64_t>
class MaskStateMachine {
    static_assert(std::is_unsigned<MaskType>::value, "MaskType must be unsigned integer");

public:
    using validator_t = std::function<bool(MaskType old_mask, MaskType new_mask)>;
    using observer_t  = std::function<void(MaskType old_mask, MaskType new_mask)>;

    struct BitInfo {
        MaskType mask;       // 单位位或位段掩码（已移位到最终位置）
        std::string name;    // 名称
        std::string desc;    // 可选描述
    };

    // ctor: initial mask 默认 0
    explicit MaskStateMachine(MaskType initial = 0) : state_{initial} {}

    // 禁用复制（原子类型不可复制语义）
    MaskStateMachine(const MaskStateMachine&) = delete;
    MaskStateMachine& operator=(const MaskStateMachine&) = delete;

    // 读取当前掩码（非阻塞、即时）
    MaskType load(std::memory_order mo = std::memory_order_acquire) const {
        return state_.load(mo);
    }

    // 非条件写（无校验）—— 原子写入（release）
    void store(MaskType v, std::memory_order mo = std::memory_order_release) {
        state_.store(v, mo);
        // note: 这里不触发 observer，因为没有 old/new 回调；若需要，使用 update_with_validator/cas_update
    }

    // 注册位/字段元信息（线程安全）
    void register_bitinfo(const BitInfo& info) {
        std::lock_guard lock(meta_mtx_);
        meta_.push_back(info);
    }

    // 打印可读的掩码解析（例如用于日志/调试）
    std::string dump(MaskType mask) const {
        std::ostringstream oss;
        oss << "mask=0x" << std::hex << mask << std::dec << " {";
        std::lock_guard lock(meta_mtx_);
        bool first = true;
        for (const auto &b : meta_) {
            if ((mask & b.mask) == b.mask) {
                if (!first) oss << ", ";
                oss << b.name;
                first = false;
            }
        }
        oss << "}";
        return oss.str();
    }

    // 注册 observer（成功变更后会异步/同步地回调——此处为同步回调）
    void subscribe(observer_t obs) {
        std::lock_guard lock(obs_mtx_);
        observers_.push_back(std::move(obs));
    }

    // 尝试原子地设置位组（或字段），可传入 validator（可选）
    // compute_new: 根据 old_mask 计算 new_mask；validator: 验证 old->new 是否合法（若未提供视为合法）
    // 返回 pair<success, prev_old_mask>
    std::pair<bool, MaskType> cas_update(
        std::function<MaskType(MaskType)> compute_new,
        std::optional<validator_t> validator = std::nullopt)
    {
        MaskType oldv = state_.load(std::memory_order_acquire);
        for (;;) {
            MaskType newv = compute_new(oldv);
            if (validator && !(*validator)(oldv, newv)) {
                return {false, oldv};
            }
            if (state_.compare_exchange_weak(oldv, newv,
                                             std::memory_order_release,
                                             std::memory_order_acquire)) {
                notify_observers(oldv, newv);
                return {true, oldv};
            }
            // compare_exchange_weak 会修改 oldv 为最新值，重试
        }
    }

    // 原子置位（无校验）
    MaskType fetch_or(MaskType bits) {
        return state_.fetch_or(bits, std::memory_order_acq_rel);
    }

    // 原子清位（无校验）
    MaskType fetch_and(MaskType bits) {
        return state_.fetch_and(bits, std::memory_order_acq_rel);
    }

    // 便利方法：设置字段（指定 offset 和 width），并可选 validator
    // width: 值所占位宽（1..bits_of(MaskType)）
    bool set_field_atomic(unsigned offset, unsigned width, MaskType value,
                          std::optional<validator_t> validator = std::nullopt)
    {
        if (offset >= bits_total() || width == 0 || offset + width > bits_total()) {
            throw std::out_of_range("offset/width overflow");
        }
        MaskType field_mask = make_field_mask(offset, width);
        auto compute = [=](MaskType oldv) -> MaskType {
            MaskType cleared = oldv & ~field_mask;
            MaskType packed  = ( (value & ((MaskType(1) << width) - 1)) << offset );
            return cleared | packed;
        };
        auto res = cas_update(compute, validator);
        return res.first;
    }

    // 读取字段
    MaskType get_field(MaskType mask, unsigned offset, unsigned width) const {
        if (offset >= bits_total() || width == 0 || offset + width > bits_total()) {
            throw std::out_of_range("offset/width overflow");
        }
        MaskType field_mask = make_field_mask(offset, width);
        return (mask & field_mask) >> offset;
    }

    // 辅助：创建字段掩码
    static constexpr MaskType make_field_mask(unsigned offset, unsigned width) {
        if (width >= bits_total()) {
            // 全域掩码
            return ~MaskType(0);
        }
        return ( ((MaskType(1) << width) - 1) << offset );
    }

    // 辅助：创建单个位掩码
    static constexpr MaskType bit_at(unsigned pos) {
        return (MaskType(1) << pos);
    }

protected:
    // 通知 observers（同步）
    void notify_observers(MaskType oldv, MaskType newv) {
        std::lock_guard lock(obs_mtx_);
        for (const auto& o : observers_) {
            try {
                o(oldv, newv);
            } catch (...) {
                // 观察者异常不应破坏状态机；记录或忽略
                // 可扩展为日志记录
            }
        }
    }

    static constexpr unsigned bits_total() {
        return sizeof(MaskType) * 8;
    }

private:
    std::atomic<MaskType> state_;
    mutable std::mutex meta_mtx_;
    std::vector<BitInfo> meta_;

    std::mutex obs_mtx_;
    std::vector<observer_t> observers_;
};

} // namespace msf
