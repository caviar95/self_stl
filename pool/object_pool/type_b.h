#pragma once

#include <string>

struct TypeB {
    int id;
    std::string name;

    std::string content;

    TypeB(int id, std::string &name, std::string &content) : id(id), name(name), content(content) {}
};
