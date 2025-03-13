#pragma once

#include "fast_mutex.h"


# ifdef _NOTHREADS
    /**
     * Helper class for class-level locking.  This is the
     * single-threaded implementation.
     */
    template <class _Host, bool _RealLock = false>
    class class_level_lock {
    public:
        /** Type that provides locking/unlocking semantics. */
        class lock {
        public:
            lock() = default;
        };

        typedef _Host volatile_type;
    };
# else
    /**
     * Helper class for class-level locking.  This is the multi-threaded
     * implementation.  The main departure from Loki ClassLevelLockable
     * is that there is an additional template parameter which can make
     * the lock not %lock at all even in multi-threaded environments.
     * See static_mem_pool.h for real usage.
     */
    template <class _Host, bool _RealLock = true>
    class class_level_lock {
        static fast_mutex _S_mtx;

    public:
        // The C++ 1998 Standard required the use of `friend' here, but
        // this requirement was considered a defect and subsequently
        // changed.  It is still used here for compatibility with older
        // compilers.
        class lock;
        friend class lock;

        /** Type that provides locking/unlocking semantics. */
        class lock {
        public:
            lock()
            {
                if (_RealLock) {
                    _S_mtx.lock();
                }
            }
            lock(const lock&) = delete;
            lock& operator=(const lock&) = delete;
            ~lock()
            {
                if (_RealLock) {
                    _S_mtx.unlock();
                }
            }
        };

        typedef volatile _Host volatile_type;
    };

    /** Partial specialization that makes null locking. */
    template <class _Host>
    class class_level_lock<_Host, false> {
    public:
        /** Type that provides locking/unlocking semantics. */
        class lock {
        public:
            lock() = default;
        };

        typedef _Host volatile_type;
    };

    template <class _Host, bool _RealLock>
    fast_mutex class_level_lock<_Host, _RealLock>::_S_mtx;
# endif // _NOTHREADS

