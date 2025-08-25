#include "event.hpp"
#include "foo.hpp"

int main() {
    Foo foo;
    EventSource src;
    src.SetCallback([&foo](int x) { foo.OnEvent(x); });

    return 0;
}