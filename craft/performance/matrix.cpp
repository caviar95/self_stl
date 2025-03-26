#include <iostream>
#include <memory>
#include <chrono>
#include <cstdlib>
#include <stdexcept>

#ifdef _WIN32
#define ALIGNED_ALLOC(alignment, size) _aligned_malloc(size, alignment)
#define ALIGNED_FREE(ptr) _aligned_free(ptr)
#else
#define ALIGNED_ALLOC(alignment, size) aligned_alloc(alignment, size)
#define ALIGNED_FREE(ptr) free(ptr)
#endif

template <size_t N>
class Matrix {
public:
    Matrix() {
        data_ = static_cast<float*>(ALIGNED_ALLOC(64, N*N*sizeof(float)));
        if (!data_) throw std::bad_alloc();
    }

    ~Matrix() {
        ALIGNED_FREE(data_);
    }

    // 禁用拷贝操作
    Matrix(const Matrix&) = delete;
    Matrix& operator=(const Matrix&) = delete;

    // 允许移动操作
    Matrix(Matrix&& other) noexcept : data_(other.data_) {
        other.data_ = nullptr;
    }

    Matrix& operator=(Matrix&& other) noexcept {
        if (this != &other) {
            ALIGNED_FREE(data_);
            data_ = other.data_;
            other.data_ = nullptr;
        }
        return *this;
    }

    void init(float val) noexcept {
        for(size_t i=0; i<N*N; ++i) data_[i] = val;
    }

    __attribute__((always_inline)) inline 
    const float* data() const noexcept { return data_; }

    __attribute__((always_inline)) inline 
    float* data() noexcept { return data_; }

    __attribute__((always_inline)) inline 
    const float* operator[](size_t row) const noexcept {
        return &data_[row*N];
    }

    __attribute__((always_inline)) inline 
    float* operator[](size_t row) noexcept {
        return &data_[row*N];
    }

private:
    float* data_;  // 原始指针管理内存
};

template <size_t N>
void matrix_multiply(const Matrix<N>& a, const Matrix<N>& b, Matrix<N>& result) {
    const float* __restrict a_data = a.data();
    const float* __restrict b_data = b.data();
    float* __restrict r_data = result.data();

    #pragma omp simd collapse(2) aligned(a_data, b_data, r_data:64)
    for(size_t i=0; i<N; ++i) {
        for(size_t j=0; j<N; ++j) {
            float sum = 0.0f;
            for(size_t k=0; k<N; ++k) {
                sum += a_data[i*N + k] * b_data[k*N + j];
            }
            r_data[i*N + j] = sum;
        }
    }
}

int main() {
    constexpr size_t N = 512;
    Matrix<N> a, b, result;
    
    a.init(2.0f);
    b.init(3.0f);

    auto start = std::chrono::high_resolution_clock::now();
    matrix_multiply(a, b, result);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Matrix " << N << "x" << N << " multiply in " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()
              << " ms\n";
    std::cout << "Sample value: " << result[0][0] << std::endl;
    
    return 0;
}
