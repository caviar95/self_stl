#pragma once

#include <mutex>

# ifdef _DEBUG
#   include <stdio.h>
#   include <stdlib.h>
/** Macro for fast_mutex assertions.  Real version (for debug mode). */
#   define _FAST_MUTEX_ASSERT(_Expr, _Msg) \
        if (!(_Expr)) { \
            fprintf(stderr, "fast_mutex::%s\n", _Msg); \
            abort(); \
        }
# else
/** Macro for fast_mutex assertions.  Fake version (for release mode). */
#   define _FAST_MUTEX_ASSERT(_Expr, _Msg) \
        ((void)0)
# endif


class fast_mutex {
    std::mutex _M_mtx_impl;
#if _FAST_MUTEX_CHECK_INITIALIZATION
    bool _M_initialized;
#endif
# ifdef _DEBUG
    bool _M_locked;
#endif

public:
    fast_mutex()
# ifdef _DEBUG
        : _M_locked(false)
#endif
    {
#if _FAST_MUTEX_CHECK_INITIALIZATION
        _M_initialized = true;
#endif
    }
    ~fast_mutex()
    {
        _FAST_MUTEX_ASSERT(!_M_locked, "~fast_mutex(): still locked");
#if _FAST_MUTEX_CHECK_INITIALIZATION
        _M_initialized = false;
#endif
    }
    void lock()
    {
#if _FAST_MUTEX_CHECK_INITIALIZATION
        if (!_M_initialized) {
            return;
        }
#endif
        _M_mtx_impl.lock();
# ifdef _DEBUG
        _FAST_MUTEX_ASSERT(!_M_locked, "lock(): already locked");
        _M_locked = true;
#endif
    }
    void unlock()
    {
#if _FAST_MUTEX_CHECK_INITIALIZATION
        if (!_M_initialized) {
            return;
        }
#endif
# ifdef _DEBUG
        _FAST_MUTEX_ASSERT(_M_locked, "unlock(): not locked");
        _M_locked = false;
#endif
        _M_mtx_impl.unlock();
    }

private:
    fast_mutex(const fast_mutex&) = delete;
    fast_mutex& operator=(const fast_mutex&) = delete;
};