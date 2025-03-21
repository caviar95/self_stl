#include <iostream>

template <typename T>
void Print(T value) {
    std::cout << value << std::endl;
}

template <typename T, size_t N>
void Print(T (&arr)[N]) {
    for (size_t i = 0; i < N; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

void TestArr() {
    int arr[3] = {1, 2, 3};
    Print(arr);
}

int main() {
    TestArr();
    return 0;
}
