// 目标接口
class EuropeanSocket {
public:
    virtual int voltage() = 0;
};

// 被适配类
class USSocket {
public:
    int usVoltage() { return 120; }
};

// 适配器
class USSocketAdapter : public EuropeanSocket {
private:
    USSocket _usSocket;
public:
    int voltage() override { return _usSocket.usVoltage(); }
};