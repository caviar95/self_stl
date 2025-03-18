#pragma once


// Sieve of Eratosthenes
// 要得到自然数n以内的全部素数，必须把不大于 的所有素数的倍数剔除，剩下的就是素数
// 给出要筛数值的范围n，找出以内的素数。先用2去筛，即把2留下，把2的倍数剔除掉；再用下一个质数，也就是3筛，把3留下，把3的倍数剔除掉；
// 接下去用下一个质数5筛，把5留下，把5的倍数剔除掉；不断重复下去

#include <vector>

namespace Prime {

const int MX = 101;
std::vector<int> is_prime(MX, 1);
std::vector<int> primes;

void sieve() {
    for (int i = 2; i < MX; ++i) {
        if (is_prime[i]) {
            primes.push_back(i);
            for (int j = i * i; j < MX; j += i) {
                is_prime[j] = 0;
            }
        }
    }
}

// Euler
void euler() {
    for (auto i = 2; i < MX; ++i) {
        if (is_prime[i]) {
            primes.push_back(i);
        }

        for (auto p : primes) {
            if (p * i > MX) {
                break;
            }

            is_prime[p * i] = 0;

            if (i % p == 0) {
                break;
            }
        }
    }
}

}
