// OCP 软件实体对扩展开放，对修改关闭
//
class Shape {
    public:
        virtual double area() const = 0;
};

class Circle : public Shape {
double radius;
    public:
double area() const override {
    return 3.14 * radius *radius;
}
};

class Square : public Shape {
double side;

    public:
        double area() const override {
            return side * side;
        }
}; 
