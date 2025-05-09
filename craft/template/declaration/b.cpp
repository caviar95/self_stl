#include "my_template.hpp"
#include "b.h"

void demo_b() {
    MyClass<long long> obj1(2000); // 调用long long特化
    obj1.print();                  // 输出: [LongLong特化] data: 3000

    MyClass<int> obj2(8);          // 共享int特化
    obj2.print();                  // 输出: [Int特化] data: 16

    MyClass obj3("Hello"); // 调用默认模板
    obj3.print();                       // 输出: [默认模板] data: Hello
}
