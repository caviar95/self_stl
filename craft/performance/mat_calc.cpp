#include <iostream>
#include <array>
#include <chrono>
#include <immintrin.h>
#include <omp.h>

inline float mm256_reduce_add_ps(__m256 v) {
    __m128 vlow = _mm256_castps256_ps128(v);
    __m128 vhigh = _mm256_extractf128_ps(v, 1);
    vlow = _mm_add_ps(vlow, vhigh);
    __m128 shuf = _mm_movehdup_ps(vlow);
    __m128 sums = _mm_add_ps(vlow, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    return _mm_cvtss_f32(sums);
}

template <size_t N>
class Matrix {
public:
    alignas(64) std::array<float, N*N> data{};

    constexpr Matrix() = default;
    
    constexpr float get(size_t i, size_t j) const noexcept { 
        return data[i*N + j]; 
    }
    
    constexpr void set(size_t i, size_t j, float val) noexcept { 
        data[i*N + j] = val; 
    }

    void fill(float val) noexcept {
        float* ptr = data.data();
        ptr = static_cast<float*>(__builtin_assume_aligned(ptr, 64));
        #pragma omp parallel for simd aligned(ptr:64)
        for(size_t i=0; i<N*N; ++i) ptr[i] = val;
    }
};

template <size_t N>
void matrix_multiply(const Matrix<N>& a, const Matrix<N>& b, Matrix<N>& result) {
    const float* __restrict a_ptr = a.data.data();
    const float* __restrict b_ptr = b.data.data();
    float* __restrict res_ptr = result.data.data();

    a_ptr = static_cast<const float*>(__builtin_assume_aligned(a_ptr, 64));
    b_ptr = static_cast<const float*>(__builtin_assume_aligned(b_ptr, 64));
    res_ptr = static_cast<float*>(__builtin_assume_aligned(res_ptr, 64));

    constexpr size_t simd_width = 8;
    
    #pragma omp parallel for collapse(2) schedule(static)
    for(size_t i=0; i<N; ++i) {
        for(size_t j=0; j<N; ++j) {
            __m256 sum_vec = _mm256_setzero_ps();
            
            // 向量化循环（显式声明k_vec）
            #pragma omp simd aligned(a_ptr, b_ptr:64)
            for(size_t k_vec=0; k_vec < N; k_vec += simd_width) {
                const __m256 a_vec = _mm256_load_ps(&a_ptr[i*N + k_vec]);
                const __m256 b_vec = _mm256_load_ps(&b_ptr[k_vec*N + j]);
                sum_vec = _mm256_fmadd_ps(a_vec, b_vec, sum_vec);
            }
            
            // 标量处理剩余元素
            float sum = mm256_reduce_add_ps(sum_vec);
            const size_t remaining_start = (N / simd_width) * simd_width;
            for(size_t k = remaining_start; k < N; ++k) {
                sum += a_ptr[i*N + k] * b_ptr[k*N + j];
            }
            
            res_ptr[i*N + j] = sum;
        }
    }
}

template <size_t N>
bool verify_result(const Matrix<N>& result, float expected) {
    constexpr float eps = 1e-3;
    for(size_t i=0; i<N; ++i) {
        for(size_t j=0; j<N; ++j) {
            if(std::abs(result.get(i,j) - expected) > eps) {
                std::cerr << "Error at (" << i << "," << j << "): " 
                          << result.get(i,j) << " vs " << expected << "\n";
                return false;
            }
        }
    }
    return true;
}

int main() {
    constexpr size_t N = 512;
    Matrix<N> a, b, result;
    a.fill(2.0f);
    b.fill(3.0f);

    auto start = std::chrono::high_resolution_clock::now();
    matrix_multiply(a, b, result);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::cout << "Time: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>
                 (end-start).count() << " ms\n";
    
    if constexpr (N <= 1024) {
        const float expected = 2.0f * 3.0f * N;
        if(verify_result(result, expected)) {
            std::cout << "Verification passed!\n";
        }
    }
    return 0;
}