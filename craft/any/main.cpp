#include "any.hpp"

int main()
{
    Any a = 42;
    assert(a.cast<int>() == 42);

    return 0;
}
