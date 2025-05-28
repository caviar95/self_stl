#include <iostream>
#include <memory>

class Shape {
public:
    virtual void show() = 0;
};


class Circle : public Shape {
public:
    void show() override {
        std::cout << "Circle" << std::endl;
    }
};

class Triangle : public Shape {
public:
    void show() override {
        std::cout << "Triangle" << std::endl;
    } 
};

enum class ConcreteShape {CIRCLE, TRIANGLE};

class ShapeFactory {
public:
    static std::unique_ptr<Shape> CreateGraph(ConcreteShape type) {
        switch (type) {
            case ConcreteShape::CIRCLE:
                return  std::make_unique<Circle>();
            case ConcreteShape::TRIANGLE:
                return std::make_unique<Triangle>();
            
            default:
                return nullptr;
        }
    }
};

int main()
{
    ShapeFactory factory{};
    Shape *t = factory.CreateGraph(ConcreteShape::CIRCLE);

    t->show();

    delete t;

    return 0;
}