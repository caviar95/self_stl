#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <memory>

// 基类
class Animal {
public:
    virtual ~Animal() = default;
    virtual void speak() = 0;
};

// 子类 Dog
class Dog : public Animal {
public:
    void speak() override {
        std::cout << "Woof!" << std::endl;
    }
};

// 子类 Cat : public Animal {
class Cat : public Animal {
public:
    void speak() override {
        std::cout << "Meow!" << std::endl;
    }
};

// 反射系统核心
class ReflectionFactory {
private:
    // 存储类型名称与构造函数的映射
    static std::map<std::string, std::function<std::shared_ptr<Animal>()>>& getRegistry() {
        static std::map<std::string, std::function<std::shared_ptr<Animal>()>> registry;
        return registry;
    }

public:
    // 注册类型
    template <typename T>
    static void registerClass(const std::string& name) {
        getRegistry()[name] = []() -> std::shared_ptr<Animal> {
            return std::make_shared<T>();
        };
    }

    // 根据类型名创建对象
    static std::shared_ptr<Animal> create(const std::string& name) {
        auto it = getRegistry().find(name);
        if (it != getRegistry().end()) {
            return it->second();
        }
        return nullptr;
    }
};

// 注册宏（简化手动注册）
#define REGISTER_ANIMAL(className) \
    ReflectionFactory::registerClass<className>(#className)

int main() {
    // 注册子类
    REGISTER_ANIMAL(Dog);
    REGISTER_ANIMAL(Cat);

    // 通过类型名动态创建对象
    auto dog = ReflectionFactory::create("Dog");
    auto cat = ReflectionFactory::create("Cat");

    if (dog) dog->speak();  // 输出: Woof!
    if (cat) cat->speak();  // 输出: Meow!

    return 0;
}