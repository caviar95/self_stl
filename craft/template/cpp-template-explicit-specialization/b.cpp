// b.cpp
#include "template.hpp"
#include "b.hpp"
#include <iostream>

void useTemplateInB() {
    MyTemplateClass<int> intObj(20);
    intObj.printData();

    MyTemplateClass<double> doubleObj(2.718);
    doubleObj.printData();

    MyTemplateClass<long long> longLongObj(987654321LL);
    longLongObj.printData();
}    