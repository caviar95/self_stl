#include <iostream>
#include <chrono>

void normal(int x) {
    volatile int y = x + 1;
}

int main() {
    auto lambda = [](int x) {
        volatile int y = x + 1;
    };

    int n = 10000000;

    auto start1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; ++i) normal(i);
    auto end1 = std::chrono::high_resolution_clock::now();

    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; ++i) lambda(i);
    auto end2 = std::chrono::high_resolution_clock::now();

    std::cout << "normal: "
              << std::chrono::duration<double, std::milli>(end1 - start1).count()
              << " ms\n";
    std::cout << "lambda: "
              << std::chrono::duration<double, std::milli>(end2 - start2).count()
              << " ms\n";
}

/*
river@DESKTOP-2ITCFKE:~/Code/self_stl/craft/lambda$ g++ vs_function.cpp 
river@DESKTOP-2ITCFKE:~/Code/self_stl/craft/lambda$ ./a.out 
normal: 23.3583 ms
lambda: 26.1386 ms
river@DESKTOP-2ITCFKE:~/Code/self_stl/craft/lambda$ ./a.out 
normal: 26.0996 ms
lambda: 21.1138 ms
river@DESKTOP-2ITCFKE:~/Code/self_stl/craft/lambda$ ./a.out 
normal: 24.4671 ms
lambda: 23.7242 ms
river@DESKTOP-2ITCFKE:~/Code/self_stl/craft/lambda$ g++ vs_function.cpp -O2
river@DESKTOP-2ITCFKE:~/Code/self_stl/craft/lambda$ ./a.out 
normal: 4.1448 ms
lambda: 3.8297 ms
river@DESKTOP-2ITCFKE:~/Code/self_stl/craft/lambda$ ./a.out 
normal: 2.8262 ms
lambda: 2.7977 ms
river@DESKTOP-2ITCFKE:~/Code/self_stl/craft/lambda$ 
*/