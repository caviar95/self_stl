#include <string.h>
#include <iostream>

void test() {
    char secret[32] = "TopSecretPassword123!";

    std::cout << "before: " << secret << std::endl;
    explicit_bzero(secret, sizeof(secret));

    std::cout << "after: " << secret << std::endl;
}

int main() {
    test();


    return 0;
}
