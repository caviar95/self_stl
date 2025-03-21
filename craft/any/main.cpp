#include "any.hpp"

int main() {
    Any a;
    a = 10;
    assert(a.cast<int>() == 110); 
}