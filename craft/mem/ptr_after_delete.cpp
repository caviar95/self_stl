#include <iostream>

using namespace std;

void test() {
    int *p = new int(10);
    cout << hex << p << endl;

    delete p;

    cout << p << endl;
}

int main() {
    test();

    return 0;
}


