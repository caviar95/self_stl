#include "mask_state_machine.hpp"
#include <iostream>

int main() {
    using Mask = uint64_t;
    using SM = msf::MaskStateMachine<Mask>;

    SM sm{0};

    // 定义偏移（示例：每子系统 8 位）
    constexpr unsigned A_OFF = 0;
    constexpr unsigned B_OFF = 8;
    constexpr unsigned W = 8;

    // 注册可读元信息（按单个位或位段）
    sm.register_bitinfo({ SM::make_field_mask(A_OFF, W), "A_field", "Subsystem A state (8bit)" });
    sm.register_bitinfo({ SM::make_field_mask(B_OFF, W), "B_field", "Subsystem B state (8bit)" });

    // 订阅变化
    sm.subscribe([](Mask oldv, Mask newv){
        std::cout << "transition: " << std::hex << oldv << " -> " << newv << std::dec << "\n";
    });

    // 简单 validator：禁止 A 从 2 -> 1
    auto validator = [](Mask oldv, Mask newv) -> bool {
        Mask oldA = (oldv >> A_OFF) & 0xFF;
        Mask newA = (newv >> A_OFF) & 0xFF;
        if (oldA == 2 && newA == 1) return false;
        return true;
    };

    // 尝试设置 A=2
    bool ok1 = sm.set_field_atomic(A_OFF, W, 2, validator);
    std::cout << "set A=2 ok=" << ok1 << ", dump=" << sm.dump(sm.load()) << "\n";

    // 尝试非法回退 A:2 -> 1 （validator 会阻止）
    bool ok2 = sm.set_field_atomic(A_OFF, W, 1, validator);
    std::cout << "set A=1 ok=" << ok2 << ", dump=" << sm.dump(sm.load()) << "\n";

    return 0;
}
