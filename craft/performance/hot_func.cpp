#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <immintrin.h>  // AVX2指令集头文件

using namespace std;
using namespace std::chrono;

//------------------------------------------
// 原始版本（含分支预测）
//------------------------------------------
int original_sum(const vector<int>& data) {
    int sum = 0;
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] % 2 == 0) {  // 分支预测失败点
            sum += data[i] * 2;
        }
    }
    return sum;
}

//------------------------------------------
// SIMD优化版本（无分支 + 向量化 + 循环展开）
//------------------------------------------
__m256i process_vector(__m256i vec) {
    // 步骤1：检测偶数（生成掩码）
    const __m256i one = _mm256_set1_epi32(1);
    const __m256i mask = _mm256_cmpeq_epi32(
        _mm256_and_si256(vec, one), 
        _mm256_setzero_si256()
    );

    // 步骤2：仅保留偶数并乘以2
    const __m256i even_values = _mm256_and_si256(vec, mask);
    return _mm256_slli_epi32(even_values, 1); // 左移1位等价于*2
}

int optimized_sum(const int* data, int size) {
    __m256i sum_vec = _mm256_setzero_si256();
    int i = 0;

    // 主循环：每次处理32个元素（4个AVX向量）
    constexpr int vec_group = 4;
    for (; i <= size - vec_group * 8; i += vec_group * 8) {
        // 循环展开：同时加载4个向量
        __m256i v0 = _mm256_loadu_si256((const __m256i*)(data + i));
        __m256i v1 = _mm256_loadu_si256((const __m256i*)(data + i + 8));
        __m256i v2 = _mm256_loadu_si256((const __m256i*)(data + i + 16));
        __m256i v3 = _mm256_loadu_si256((const __m256i*)(data + i + 24));

        // 向量处理流水线
        v0 = process_vector(v0);
        v1 = process_vector(v1);
        v2 = process_vector(v2);
        v3 = process_vector(v3);

        // 累加结果
        sum_vec = _mm256_add_epi32(sum_vec, v0);
        sum_vec = _mm256_add_epi32(sum_vec, v1);
        sum_vec = _mm256_add_epi32(sum_vec, v2);
        sum_vec = _mm256_add_epi32(sum_vec, v3);
    }

    // 尾循环：处理剩余不足32个元素
    for (; i <= size - 8; i += 8) {
        __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
        sum_vec = _mm256_add_epi32(sum_vec, process_vector(v));
    }

    // 合并向量结果
    int temp[8];
    _mm256_storeu_si256((__m256i*)temp, sum_vec);
    int sum = temp[0] + temp[1] + temp[2] + temp[3] 
            + temp[4] + temp[5] + temp[6] + temp[7];

    // 处理剩余元素（标量模式）
    for (; i < size; ++i) {
        sum += (data[i] & 1) ? 0 : data[i] * 2; // 无分支写法
    }
    return sum;
}

//------------------------------------------
// 测试用例与性能对比
//------------------------------------------
int main() {
    // 生成测试数据（100万随机整数）
    constexpr int size = 1000000;
    vector<int> data(size);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 10000);

    for (int& num : data) {
        num = dis(gen);
    }

    // 验证正确性
    const int original = original_sum(data);
    const int optimized = optimized_sum(data.data(), data.size());
    cout << "Validation: " 
         << (original == optimized ? "Pass" : "Fail") 
         << endl;

    // 性能测试（原始版本）
    auto start = high_resolution_clock::now();
    volatile int tmp1 = original_sum(data); // volatile防止优化
    auto end = high_resolution_clock::now();
    auto duration_original = duration_cast<microseconds>(end - start).count();

    // 性能测试（优化版本）
    start = high_resolution_clock::now();
    volatile int tmp2 = optimized_sum(data.data(), data.size());
    end = high_resolution_clock::now();
    auto duration_optimized = duration_cast<microseconds>(end - start).count();

    // 输出结果
    cout << "Original time: " << duration_original << "μs\n"
         << "Optimized time: " << duration_optimized << "μs\n"
         << "Speedup: " 
         << (double)duration_original / duration_optimized << "x\n";

    return 0;
}
