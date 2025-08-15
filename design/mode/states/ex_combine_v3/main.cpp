#include "state_machine.h"
#include <iostream>

int main() {
    // 默认：4 台设备、每台 8 位、总计 32 位 mask（兼容你现有方案）
    using SM = StateMachine<uint32_t, 4, 8>;
    SM sm;

    // 准备一些业务映射（mask -> finalState）
    // 数值编码：每台设备的 8 位是一个数值（0..255），而不是 8 个 one-hot 位
    // 例如：设备状态向量 [A=3, B=10, C=0, D=7]
    SM::Mask m1 = SM::makeMask({3, 10, 0, 7});
    SM::Mask m2 = SM::makeMask({5, 10, 0, 7});
    SM::Mask m3 = SM::makeMask({5, 12, 1, 7});

    sm.addMapping(m1, "FINAL_ALPHA");
    sm.addMapping(m2, "FINAL_BETA");
    sm.addMapping(m3, "FINAL_GAMMA");

    sm.printStateTable();

    // 当前状态（仍然用同样的 32 位数值编码）
    SM::Mask cur = SM::makeMask({4, 9, 0, 7});

    // 业务上希望到达 m2（BETA）
    SM::Mask desired = m2;

    // 计算下一掩码（禁止降级：逐设备取 max(cur_i, desired_i)）
    SM::Mask next = sm.getNextMask(cur, desired);

    auto arrCur = SM::decodeMask(cur);
    auto arrDes = SM::decodeMask(desired);
    auto arrNext = SM::decodeMask(next);

    std::cout << "\nTransition (no downgrade):\n";
    for (size_t i = 0; i < arrCur.size(); ++i) {
        std::cout << "  Dev" << i << ": " << arrCur[i]
                  << " -> desired " << arrDes[i]
                  << " -> next " << arrNext[i] << "\n";
    }

    std::cout << "canTransitionNoDowngrade(cur, desired) = "
              << (SM::canTransitionNoDowngrade(cur, desired) ? "true" : "false")
              << "\n";

    // 用 next 作为 key 查 finalState
    std::cout << "FinalState(next) = " << sm.findFinalState(next) << "\n";

    // 验证降级被阻止：尝试把 A 从 6 降到 5（不允许）
    SM::Mask cur2 = SM::makeMask({6, 10, 0, 7});
    SM::Mask des2 = SM::makeMask({5, 12, 1, 7}); // A 降级（6 -> 5），其余升级
    SM::Mask next2 = sm.getNextMask(cur2, des2);
    auto arrCur2 = SM::decodeMask(cur2);
    auto arrDes2 = SM::decodeMask(des2);
    auto arrNext2 = SM::decodeMask(next2);

    std::cout << "\nDowngrade test (A: 6 -> 5 should be blocked):\n";
    for (size_t i = 0; i < arrCur2.size(); ++i) {
        std::cout << "  Dev" << i << ": " << arrCur2[i]
                  << " -> desired " << arrDes2[i]
                  << " -> next " << arrNext2[i] << "\n";
    }
    std::cout << "FinalState(next2) = " << sm.findFinalState(next2) << "\n";

    return 0;
}
