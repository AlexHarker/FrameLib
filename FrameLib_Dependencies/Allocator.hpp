
#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <cstdlib>

#include "SIMDSupport.hpp"

namespace impl
{
    typedef void *(*allocate_function)(size_t);
    typedef void (*free_function)(void *);
};

// A template for wrapping functions as an allocator

template<impl::allocate_function alloc, impl::free_function dealloc>
struct function_allocator
{
    template <typename T>
    T* allocate(size_t size) { return reinterpret_cast<T*>(alloc(size * sizeof(T))); }
    
    template <typename T>
    void deallocate(T *ptr) { dealloc(ptr); }
};

using malloc_allocator = function_allocator<malloc, free>;

// Aligned allocator

struct aligned_allocator
{
    template <typename T>
    T* allocate(size_t size) { return allocate_aligned<T>(size); }
    
    template <typename T>
    void deallocate(T *ptr) { deallocate_aligned(ptr); }
};

#endif
