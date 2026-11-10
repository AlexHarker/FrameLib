
#ifndef FRAMELIB_FLUIDALLOCATOR_H
#define FRAMELIB_FLUIDALLOCATOR_H

#include "FrameLib_DSP.h"

#include <new>

// A foonathan memory RawAllocator that adapts a FrameLib allocator for use as a fluid::Allocator

struct FrameLib_FluidAllocator
{
    using is_stateful = std::true_type;

    explicit FrameLib_FluidAllocator(FrameLib_DSP::Allocator allocator) : mAllocator(allocator) {}

    void *allocate_node(std::size_t size, std::size_t alignment)
    {
        if (alignment > FrameLib_GlobalAllocator::getAlignment())
            throw std::bad_alloc();
        void *ptr = mAllocator.allocate<char>(size);
        if (!ptr)
            throw std::bad_alloc();
        return ptr;
    }

    void deallocate_node(void *ptr, std::size_t, std::size_t) noexcept
    {
        char *memory = static_cast<char *>(ptr);
        mAllocator.deallocate(memory);
    }

    FrameLib_DSP::Allocator mAllocator;
};

#endif
