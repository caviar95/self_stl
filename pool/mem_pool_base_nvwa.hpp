#pragma once

#include <stddef.h> // size_t
#include "nvwa.h"
#include "cpp_features.h"



# ifdef _MEM_POOL_USE_MALLOC
#include <stdlib.h> // malloc/free
#   define _MEM_POOL_ALLOCATE(_Sz)    malloc(_Sz)
#   define _MEM_POOL_DEALLOCATE(_Ptr) free(_Ptr)
# else
#include <new> // std::bad_alloc
#   define _MEM_POOL_ALLOCATE(_Sz)    ::operator new((_Sz), std::nothrow)
#   define _MEM_POOL_DEALLOCATE(_Ptr) ::operator delete(_Ptr)
# endif


class mem_pool_base {
public:
    virtual ~mem_pool_base() {}
    virtual void recycle() = 0;
    static void *alloc_sys(size_t size) {
        return _MEM_POOL_ALLOCATE(size);
    }
    static void dealloc_sys(void *ptr) {
        _MEM_POOL_DEALLOCATE(ptr);
    }

    /** Structure to store the next available memory block. */
    struct _Block_list {
        _Block_list* _M_next;   ///< Pointer to the next memory block
    };

private:
    // remove copy semantics
    mem_pool_base(const mem_pool_base&) = delete;
    mem_pool_base& operator=(const mem_pool_base&) = delete;
};
