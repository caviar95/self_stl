// a.cpp
#include "template.hpp"
#include "a.hpp"
#include <iostream>

void useTemplateInA() {
    MyTemplateClass<int> intObj(10);
    intObj.printData();

    MyTemplateClass<double> doubleObj(3.14);
    doubleObj.printData();

    MyTemplateClass<long long> longLongObj(123456789LL);
    longLongObj.printData();
}    