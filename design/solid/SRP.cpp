// SRP 一个类应该只有一个引起它变化的原因
// 比如，只对一种事物的活动感兴趣

// not srp
class Order {
    public:
        void CreateOrder();
        void SendEmail();
};

// srp principle
class Order {
    public:
        void CreateOrder();
};

class EmailNotifier {
    public:
        void SendEmail();
};
