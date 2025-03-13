#pragma once

// Not all compilers provide these macros; they need to be defined as
// follows to get a valid expression in preprocessing when not available.
#ifndef __has_cpp_attribute
#define __has_cpp_attribute(x) 0
#endif
#ifndef __has_extension
#define __has_extension(x) 0
#endif
#ifndef __has_feature
#define __has_feature(x) 0
#endif
#ifndef __has_include
#define __has_include(x) 0
#endif

// A convenient macro for enable_if constraints
#define NVWA_CXX11_REQUIRES(...)  \
    typename std::enable_if<(__VA_ARGS__), int>::type = 0

#define _DELETED = delete
#define _FINAL final
#define _OVERRIDE override
#define _NOEXCEPT noexcept
#define _NOEXCEPT_(x) noexcept(x)
#define _NULLPTR nullptr
#define _THREAD_LOCAL thread_local
#define _FALLTHROUGH [[fallthrough]]
#define _ASSUME(expr) [[assume(expr)]]
