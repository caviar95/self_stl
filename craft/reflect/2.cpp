#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/member.hpp>
#include <iostream>

BOOST_TYPE_ERASURE_MEMBER((has_speak), speak, void());

using AnyAnimal = boost::type_erasure::any<has_speak<void()>>;

class Dog {
public:
    void speak() { std::cout << "Woof!" << std::endl; }
};

class Cat {
public:
    void speak() { std::cout << "Meow!" << std::endl; }
};

int main() {
    AnyAnimal animal1 = Dog();
    AnyAnimal animal2 = Cat();

    animal1.speak();  // 输出: Woof!
    animal2.speak();  // 输出: Meow!
    return 0;
}