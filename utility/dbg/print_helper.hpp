#pragma once

#include <string>
#include <iostream>

#define STRUCT_MEMBERS \
    X(int, a) \
    X(double, b) \
    X(std::string, c)

struct MyStruct {
#define X(type, name) type name;
    STRUCT_MEMBERS
#undef X
};

std::ostream& operator<<(std::ostream& os, const MyStruct& s) {
#define X(type, name) os << #name << ": " << s.name << ", ";
    STRUCT_MEMBERS

#undef X

    return os;
}
