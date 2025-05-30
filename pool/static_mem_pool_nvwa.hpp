#pragma once

#include <new> // for std::bad_alloc
#include <stdexcept> // for std::runtime_error
#include <vector>
#include <assert.h>
#include <stddef.h> // for size_t
#include <algorithm> // for std::for_each
#include <iterator>
#include "nvwa.h"
#include "cpp_features.h"
#include "class_level_lock.h"
#include "mem_pool_base_nvwa.hpp"
#include "cont_ptr_utils.h" // for delete_object


/* Defines the macro for debugging output */
# ifdef _STATIC_MEM_POOL_DEBUG
#   include <iostream>
#   define _STATIC_MEM_POOL_TRACE(_Lck, _Msg) \
        { \
            if (_Lck) { \
                static_mem_pool_set::lock guard; \
                std::cerr << "static_mem_pool: " << _Msg << std::endl; \
            } else { \
                std::cerr << "static_mem_pool: " << _Msg << std::endl; \
            } \
        }
# else
#   define _STATIC_MEM_POOL_TRACE(_Lck, _Msg) \
        ((void)0)
# endif

class static_mem_pool_set {
public:
    typedef class_level_lock<static_mem_pool_set>::lock lock;
    static static_mem_pool_set& instance() {
        lock guard;
        static static_mem_pool_set _S_instance;
        return _S_instance;
    }

    void recycle() {
        _STATIC_MEM_POOL_TRACE(false, "Memory pools are being recycled");
        container_type::iterator end = _M_memory_pool_set.end();
        for (container_type::iterator
                i  = _M_memory_pool_set.begin();
                i != end; ++i) {
            (*i)->recycle();
        }
    }

    void add(mem_pool_base* pool) {
        lock guard;
        _M_memory_pool_set.push_back(pool);
    }

private:
    static_mem_pool_set() {
        _STATIC_MEM_POOL_TRACE(false, "The static_mem_pool_set is created");
    }
    ~static_mem_pool_set() {
        std::for_each(_M_memory_pool_set.rbegin(), _M_memory_pool_set.rend(), delete_object());
        _STATIC_MEM_POOL_TRACE(false, "The static_mem_pool_set is destroyed");
    }
    typedef std::vector<mem_pool_base*> container_type;
    container_type _M_memory_pool_set;

    // forbid copying
    static_mem_pool_set(const static_mem_pool_set&) = delete;
    const static_mem_pool_set& operator=(const static_mem_pool_set&) = delete;
};

// Singleton class template to manage the allocation/deallocation of memory blocks of one specific size.
template <size_t _Sz, int _Gid = -1>
class static_mem_pool : public mem_pool_base {
    typedef typename class_level_lock<static_mem_pool<_Sz, _Gid>, (_Gid < 0)>
            ::lock lock;
public:
    // Gets the instance of the static memory pool.  It will create the instance if it does not already exist.
    // Generally this function is now not needed.
    static static_mem_pool& instance()
    {
        lock guard;
        if (!_S_instance_p) {
            _S_instance_p = _S_create_instance();
        }
        return *_S_instance_p;
    }
    // Gets the known instance of the static memory pool.
    // The instance must already exist.  Generally the static initializer of the template guarantees it.
    static static_mem_pool& instance_known()
    {
        assert(_S_instance_p != nullptr);
        return *_S_instance_p;
    }
    /**
     * Allocates memory and returns its pointer.  The template will try
     * to get it from the memory pool first, and request memory from the
     * system if there is no free memory in the pool.
     *
     * @return  pointer to allocated memory if successful; null
     *          otherwise
     */
    void* allocate()
    {
        {
            lock guard;
            if (_S_memory_block_p) {
                void* result = _S_memory_block_p;
                _S_memory_block_p = _S_memory_block_p->_M_next;
                return result;
            }
        }
        return _S_alloc_sys(_S_align(_Sz));
    }
    /**
     * Deallocates memory by putting the memory block into the pool.
     *
     * @param ptr  pointer to memory to be deallocated
     */
    void deallocate(void* ptr)
    {
        assert(ptr != nullptr);
        lock guard;
        _Block_list* block = reinterpret_cast<_Block_list*>(ptr);
        block->_M_next = _S_memory_block_p;
        _S_memory_block_p = block;
    }
    virtual void recycle() override;

private:
    static_mem_pool()
    {
        _STATIC_MEM_POOL_TRACE(true, "static_mem_pool<" << _Sz << ','
                                     << _Gid << "> is created");
    }
    ~static_mem_pool()
    {
#ifdef _DEBUG
        // Empty the pool to avoid false memory leakage alarms.  This is
        // generally not necessary for release binaries.
        _Block_list* block = _S_memory_block_p;
        while (block) {
            _Block_list* next = block->_M_next;
            dealloc_sys(block);
            block = next;
        }
        _S_memory_block_p = nullptr;
#endif
        _S_instance_p = nullptr;
        _S_destroyed = true;
        _STATIC_MEM_POOL_TRACE(false, "static_mem_pool<" << _Sz << ','
                                      << _Gid << "> is destroyed");
    }
    static size_t _S_align(size_t size)
    {
        return size >= sizeof(_Block_list) ? size : sizeof(_Block_list);
    }
    static void* _S_alloc_sys(size_t size);
    static static_mem_pool* _S_create_instance();

    static bool _S_destroyed;
    static static_mem_pool* _S_instance_p;
    static mem_pool_base::_Block_list* _S_memory_block_p;

    /* Forbid their use */
    static_mem_pool(const static_mem_pool&) = delete;
    const static_mem_pool& operator=(const static_mem_pool&) = delete;
};

template <size_t _Sz, int _Gid>
    bool static_mem_pool<_Sz, _Gid>::_S_destroyed = false;
template <size_t _Sz, int _Gid>
    mem_pool_base::_Block_list* static_mem_pool<_Sz, _Gid>::_S_memory_block_p = nullptr;
template <size_t _Sz, int _Gid>
    static_mem_pool<_Sz, _Gid>*static_mem_pool<_Sz, _Gid>::_S_instance_p = _S_create_instance();

/**
 * Recycles half of the free memory blocks in the memory pool to the
 * system.  It is called when a memory request to the system (in other
 * instances of the static memory pool) fails.
 */
template <size_t _Sz, int _Gid>
void static_mem_pool<_Sz, _Gid>::recycle()
{
    // Only here the global lock in static_mem_pool_set is obtained
    // before the pool-specific lock.  However, no race conditions are
    // found so far.
    lock guard;
    _Block_list* block = _S_memory_block_p;
    while (block) {
        if (_Block_list* temp = block->_M_next) {
            _Block_list* next = temp->_M_next;
            block->_M_next = next;
            dealloc_sys(temp);
            block = next;
        } else {
            break;
        }
    }
    _STATIC_MEM_POOL_TRACE(false, "static_mem_pool<" << _Sz << ','
                                  << _Gid << "> is recycled");
}

template <size_t _Sz, int _Gid>
void* static_mem_pool<_Sz, _Gid>::_S_alloc_sys(size_t size)
{
    static_mem_pool_set::lock guard;
    void* result = mem_pool_base::alloc_sys(size);
    if (!result) {
        static_mem_pool_set::instance().recycle();
        result = mem_pool_base::alloc_sys(size);
    }
    return result;
}

template <size_t _Sz, int _Gid>
static_mem_pool<_Sz, _Gid>* static_mem_pool<_Sz, _Gid>::_S_create_instance()
{
    if (_S_destroyed) {
        throw std::runtime_error("dead reference detected");
    }

    static_mem_pool_set::instance();  // Force its creation
    static_mem_pool* inst_p = new static_mem_pool();
    try
    {
        static_mem_pool_set::instance().add(inst_p);
    }
    catch (...)
    {
        _STATIC_MEM_POOL_TRACE(true,
                "Exception occurs in static_mem_pool_set::add");
        // The strange cast below is to work around a bug in GCC 2.95.3
        delete static_cast<mem_pool_base*>(inst_p);
        throw;
    }
    return inst_p;
}

// Declares the normal (throwing) allocation and deallocation functions. This macro uses the default group.
#define DECLARE_STATIC_MEM_POOL(_Cls) \
public: \
    static void* operator new(size_t size) \
    { \
        assert(size == sizeof(_Cls)); \
        void* ptr; \
        ptr = static_mem_pool<sizeof(_Cls)>:: \
                               instance_known().allocate(); \
        if (ptr == nullptr) \
            throw std::bad_alloc(); \
        return ptr; \
    } \
    static void operator delete(void* ptr) \
    { \
        if (ptr) \
            static_mem_pool<sizeof(_Cls)>:: \
                           instance_known().deallocate(ptr); \
    }

// Declares the nothrow allocation and deallocation functions.  This macro uses the default group.
#define DECLARE_STATIC_MEM_POOL__NOTHROW(_Cls) \
public: \
    static void* operator new(size_t size) _NOEXCEPT \
    { \
        assert(size == sizeof(_Cls)); \
        return static_mem_pool<sizeof(_Cls)>:: \
                              instance_known().allocate(); \
    } \
    static void operator delete(void* ptr) \
    { \
        if (ptr) \
            static_mem_pool<sizeof(_Cls)>:: \
                           instance_known().deallocate(ptr); \
    }

// Declares the normal (throwing) allocation and deallocation functions. Users need to specify a group ID.
#define DECLARE_STATIC_MEM_POOL_GROUPED(_Cls, _Gid) \
public: \
    static void* operator new(size_t size) \
    { \
        assert(size == sizeof(_Cls)); \
        void* ptr; \
        ptr = static_mem_pool<sizeof(_Cls), (_Gid)>:: \
                               instance_known().allocate(); \
        if (ptr == nullptr) \
            throw std::bad_alloc(); \
        return ptr; \
    } \
    static void operator delete(void* ptr) \
    { \
        if (ptr) \
            static_mem_pool<sizeof(_Cls), (_Gid)>:: \
                           instance_known().deallocate(ptr); \
    }

// Declares the nothrow allocation and deallocation functions.  Users need to specify a group ID.
#define DECLARE_STATIC_MEM_POOL_GROUPED__NOTHROW(_Cls, _Gid) \
public: \
    static void* operator new(size_t size) _NOEXCEPT \
    { \
        assert(size == sizeof(_Cls)); \
        return static_mem_pool<sizeof(_Cls), (_Gid)>:: \
                              instance_known().allocate(); \
    } \
    static void operator delete(void* ptr) \
    { \
        if (ptr) \
            static_mem_pool<sizeof(_Cls), (_Gid)>:: \
                           instance_known().deallocate(ptr); \
    }
