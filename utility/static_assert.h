#pragma once

#define STATIC_ASSERT(_Expr, _Msg) static_assert(_Expr, #_Msg)
// 或者
/*
template <bool> struct compile_time_error;
template <>     struct compile_time_error<true> {};

#define STATIC_ASSERT(_Expr, _Msg) \
    { \
        NVWA::compile_time_error<((_Expr) != 0)> ERROR_##_Msg; \
        (void)ERROR_##_Msg; \
    }
 */

template <typename T>
struct always_false {
    static const bool value = false;
};
