#include <boost/pfr.hpp>

#include <iostream>
#include <string>

struct MyStruct {
    int a;
    double b;
    std::string c;

};

int main() {
    MyStruct s{1, 2.5, "Hellp"};

    boost::pfr::for_each_field(s, [](const auto& field, std::size_t idx) {
        std::cout << "Field " << idx << ": " << field << std::endl;
    });

    return 0;
}