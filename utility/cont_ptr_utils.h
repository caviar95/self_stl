#pragma once

// Functor to return objects pointed by a container of pointers.
struct dereference {
    template <typename _Tp>
    const _Tp& operator()(const _Tp* ptr) const
    {
        return *ptr;
    }
};

// Functor to compare objects pointed by a container of pointers.
struct dereference_less {
    template <typename _Pointer>
    bool operator()(const _Pointer& ptr1, const _Pointer& ptr2) const
    {
        return *ptr1 < *ptr2;
    }
};

// Functor to delete objects pointed by a container of pointers.
struct delete_object {
    template <typename _Tp>
    void operator()(_Tp* ptr) const
    {
        delete ptr;
    }
};

// Functor to output objects pointed by a container of pointers.
template <typename _OutputStrm, typename _StringType = const char*>
struct output_object {
    output_object(_OutputStrm& outs, const _StringType& sep)
        : _M_outs(outs), _M_sep(sep)
    {
    }

    template <typename _Pointer>
    void operator()(const _Pointer& ptr) const
    {
        _M_outs << *ptr << _M_sep;
    }

private:
    _OutputStrm& _M_outs;
    _StringType  _M_sep;
};
