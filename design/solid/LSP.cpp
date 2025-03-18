// LSP 子类型必须能够替换掉它们的父类型


class Bird {
public:
    virtual void fly() = 0;
};

class Penguin : public Bird {
public:
    void fly() override {
        // 违反LSP，penguin不能飞
    }
};

// 应该将fly方法放在一个单独的接口中，只有可以飞的鸟才实现这个接口
