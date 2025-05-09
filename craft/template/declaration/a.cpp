#include "my_template.hpp"
#include "a.h"


void demo_a() {
    MyClass<int> obj1(5);      // 调用int特化
    obj1.print();              // 输出: [Int特化] data: 10

    MyClass<double> obj2(9.0); // 调用double特化
    obj2.print();              // 输出: [Double特化] data: 3
}