
#ifndef FRAMELIB_MEMORY_H
#define FRAMELIB_MEMORY_H

#include "tlsf.h"

// FIX - do alignment improvements ?? (be better as part of allocator directly)
// FIX - threadsafety?
// FIX - expand the block and do free heuristics differently/for malloc (always free large blocks for instance)

class FrameLib_Memory
{
 
public:
    
    // N.B. - alignment must be a power of two
    
    static const size_t alignment = 16;
    
private:
    
   
    static const int numLocalFreeBlocks = 8;
    
    struct FreeBlock
    {
        FreeBlock() : mMemory(NULL), mSize(0), mPrev(NULL), mNext(NULL) {}
        
        void *mMemory;
        size_t mSize;
        
        FreeBlock *mPrev;
        FreeBlock *mNext;
    };
    
public:

    FrameLib_Memory()
    {
        size_t initialSize = 1024 * 1024 * 128;
        mMemory = malloc(initialSize);
        mTLSF = tlsf_create_with_pool(mMemory, initialSize);
        
        for (unsigned long i = 0; i < (numLocalFreeBlocks - 1); i++)
        {
            mFreeLists[i + 1].mPrev = mFreeLists + i;
            mFreeLists[i].mNext = mFreeLists + i + 1;
        }
        
        mTop = mFreeLists;
        mTail = mFreeLists + (numLocalFreeBlocks - 1);
        mTop->mPrev = NULL;
        mTail->mNext = NULL;
    }
    
    ~FrameLib_Memory()
    {
        tlsf_destroy(mTLSF);
        free(mMemory);
    }
    
    void *alloc(size_t size)
    {
        // N.B. - all memory should be aligned to alignment
        
        //size_t maxSize = (size * 5) >> 2;
        //size_t maxSize = (size * 3) >> 1;
        size_t maxSize = size << 1;
        
        for (FreeBlock *block = mTop; block && block->mMemory; block = block->mNext)
            if (block->mSize >= size && block->mSize <= maxSize)
                return removeBlock(block);
        
        // FIX - free memory if there is an error...
        
        return tlsf_memalign(mTLSF, alignment, size);
        //return tlsf_malloc(mTLSF, size);
    }
    
    void dealloc(void *ptr)
    {
        if (mTail->mMemory)
            tlsf_free(mTLSF, mTail->mMemory);
        
        mTail->mPrev->mNext = NULL;
        mTail->mNext = mTop;
        mTail->mMemory = ptr;
        mTail->mSize = tlsf_block_size(ptr);
        mTop->mPrev = mTail;
        mTail->mNext = mTop;
        mTop = mTail;
        mTail = mTop->mPrev;
        mTop->mPrev = NULL;
    }
    
    static size_t alignSize(size_t x)
    {
        return (x + (alignment - 1)) & ~(alignment - 1);
    }
    
private:
    
    void *removeBlock(FreeBlock *block)
    {
        void *ptr = block->mMemory;
        block->mMemory = NULL;
        
        if (block == mTail)
            return ptr;
        
        if (block == mTop)
        {
            block->mNext->mPrev = NULL;
            mTop = block->mNext;
        }
        else
        {
            block->mNext->mPrev = block->mPrev;
            block->mPrev->mNext = block->mNext;
        }
        
        mTail->mNext = block;
        block->mPrev = mTail;
        mTail = block;
        
        return ptr;
    }
    
    tlsf_t mTLSF;
    void *mMemory;
    
    FreeBlock mFreeLists[numLocalFreeBlocks];
    FreeBlock *mTop;
    FreeBlock *mTail;
};

#endif
