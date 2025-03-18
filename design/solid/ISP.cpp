// ISP 客户端不能依赖它不需要的接口

class Printer {
public:
    virtual void print() = 0;
    virtual void scan() = 0;
};

class SimplePrinter : public Printer {
public:
    void print() override {
        // 实现打印功能
    }
    void scan() override {
        // 无法实现，或者没必要实现
    }
};

// 修改
class Printer {
public:
    virtual void print() = 0;
};

class Scanner {
public:
    virtual void scan() = 0;
};

class SimplePrinter : public Printer {
public:
    void print() override {
        // 实现打印功能
    }
};

class SimpleScanner : public Scanner {
public:
    void scan() override {
        // 实现扫描功能
    }
};
