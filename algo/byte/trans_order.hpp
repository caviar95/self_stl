#pragma once

#include <cstdint>
#include <type_traits>
#include <cstring>

enum class ByteOrder {
    LittleEndian,
    BigEndian
};

inline ByteOrder GetSystemByteOrder() {
    uint16_t x = 0x0102;
    return (*reinterpret_cast<uint8_t*>(&x) == 0x01) ? ByteOrder::BigEndian : ByteOrder::LittleEndian;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type ByteSwap(T value) {
    static_assert(std::is_unsigned_v<T>, "Use only with unsigned bytes");

    T result{};

    uint8_t *src = reinterpret_cast<uint8_t*>(&value);
    uint8_t *dst = reinterpret_cast<uint8_t*>(&result);

    for (size_t i = 0; i < sizeof(T); ++i) {
        dst[i = src[sizeof(T) - 1 - i];
    }

    return result;
}

template <typename T>
T ToBigEndian(T value) {
    return (GetSystemByteOrder() == ByteOrder::LittleEndian) ? ByteSwap(value) : value;
}

template <typename T>
T ToLittleEndian(T value) {
    return (GetSystemByteOrder() == ByteOrder::LittleEndian) ? value : ByteSwap(value);
}

