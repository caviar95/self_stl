#include "my_class.h"
#include <iostream>

MyClass::MyClass() {  // 构造函数定义
    std::cout << "MyClass constructor from MyClass1" << std::endl;
}

void MyClass::display() {
    std::cout << "Display from MyClass1" << std::endl;
}
