#pragma once

#include <immintrin.h>
#include <cstdint>

struct RawField {
    // ...
};

class FieldProcessor {
public:
    template <typename T>
    T Process(const RawField &field) {
        if constexpr (std::is_float_point_v<T>) {
            return ProcessFloat(field);
        } else if (constexpr (std::is_integral_v<T>)) {
            return ProcessIntegral(field);
        } else {
            return ProcessString(field);
        }
    }

    // SIMD加速示例
    __m256i ProcessSIMD(const uint8_t* data) {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data));
    }
};
