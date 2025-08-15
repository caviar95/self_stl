#include "my_class.h"
#include <iostream>

MyClass::MyClass() {  // 构造函数定义
    std::cout << "MyClass2 constructor from MyClass2" << std::endl;
}

void MyClass::display() {
    std::cout << "Display from MyClass2" << std::endl;
}
