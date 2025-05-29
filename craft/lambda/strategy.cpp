#include <iostream>
#include <functional>

// 策略执行器，持有一个策略函数
class Context {
    std::function<int(int,int)> strategy;

public:
    Context(std::function<int(int,int)> strat) : strategy(strat) {}

    void setStrategy(std::function<int(int,int)> strat) {
        strategy = strat;
    }

    int execute(int a, int b) {
        return strategy(a,b);
    }
};

int main() {
    Context ctx([](int a, int b) { return a + b; });  // 初始策略是加法

    std::cout << ctx.execute(3,4) << "\n";  // 输出7

    // 动态替换策略为乘法
    ctx.setStrategy([](int a, int b) { return a * b; });

    std::cout << ctx.execute(3,4) << "\n";  // 输出12

    // 还可以捕获外部状态，定义复杂策略
    int factor = 10;
    ctx.setStrategy([factor](int a, int b) {
        return (a + b) * factor;
    });

    std::cout << ctx.execute(3,4) << "\n";  // 输出70

    return 0;
}