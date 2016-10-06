
#include "FrameLib_Memory.h"

// Utility

// FIX - THE SECOND OF THESE FUNCTIONS IS SLOW - cache mishit?

// N.B. - alignment must be a power of two

static const size_t alignment = 16;

size_t alignedSize(size_t x)    { return (x + (alignment - 1)) & ~(alignment - 1); }
size_t blockSize(void* ptr)     { return (*(size_t *) (((unsigned char*) ptr) - sizeof(size_t))) & ~(0x3); }


// Memory Pools

FrameLib_MainAllocator::Pool::Pool(tlsf_t tlsf, size_t size) : mNext(NULL), mTLSF(tlsf), mTSLFPool(NULL)
{
    // FIX - pool alignment

    size_t actualSize = alignedSize(size) + alignedSize(tlsf_pool_overhead()) + alignedSize(tlsf_alloc_overhead());
                                                                                            
    mMemory = malloc(actualSize);
    
    if (mMemory)
        mTSLFPool = tlsf_add_pool(mTLSF, mMemory, actualSize);
}

FrameLib_MainAllocator::Pool::~Pool()
{
    if (mTSLFPool)
        tlsf_remove_pool(mTLSF, mTSLFPool);
    free(mMemory);
}

// The Main Allocator (has no threadsafety)

FrameLib_MainAllocator::FrameLib_MainAllocator() : mFirstPool(NULL), mLastPool(NULL), mOSAllocated(0), mAllocated(0)
{
    mTLSF = tlsf_create(malloc(tlsf_size()));
    addPool(initialSize);
}

FrameLib_MainAllocator::~FrameLib_MainAllocator()
{
    Pool *pool = mFirstPool;

    while (pool)
    {
        Pool *next = pool->mNext;
        delete pool;
        pool = next;
    }

    tlsf_destroy(mTLSF);
    free(mTLSF);
}

void *FrameLib_MainAllocator::alloc(size_t size)
{
    void *ptr = tlsf_memalign(mTLSF, alignment, size);
    
    if (!ptr)
    {
        // FIX - for now allocate double the necessary size (which will always work)
        
        size_t poolSize = size << 1;
        addPool(poolSize <= growSize ? growSize : poolSize);
        ptr = tlsf_memalign(mTLSF, alignment, size);
    }
    
    if (ptr)
        mAllocated += tlsf_block_size(ptr);
    
    return ptr;
}

void FrameLib_MainAllocator::dealloc(void *ptr)
{
    mAllocated -= tlsf_block_size(ptr);
    tlsf_free(mTLSF, ptr);
}

void FrameLib_MainAllocator::addPool(size_t size)
{
    Pool *pool = new Pool(mTLSF, size);
    
    mOSAllocated += size;
    
    if (mLastPool)
    {
        mLastPool->mNext = pool;
        mLastPool = pool;
    }
    else
        mFirstPool = mLastPool = pool;
}

// The Global Allocator (adds threadsaftey to the main allocator)

FrameLib_MainAllocator *FrameLib_GlobalAllocator::acquire()
{
    mLock.acquire();
    return &mAllocator;
}

bool FrameLib_GlobalAllocator::release(FrameLib_MainAllocator **allocator)
{
    if (*allocator != &mAllocator)
        return false;
    
    mLock.release();
    *allocator = NULL;
    return true;
}

void *FrameLib_GlobalAllocator::alloc(size_t size)
{
    mLock.acquire();
    void *ptr = mAllocator.alloc(size);
    mLock.release();
    
    return ptr;
}

void FrameLib_GlobalAllocator::dealloc(void *ptr)
{
    mLock.acquire();
    mAllocator.dealloc(ptr);
    mLock.release();
}


// Local Storage

FrameLib_LocalAllocator::Storage::Storage(const char *name, FrameLib_LocalAllocator *allocator) : mData(NULL), mSize(0), mMaxSize(0), mCount(1), mAllocator(allocator)
{
    mName = strdup(name);
}

FrameLib_LocalAllocator::Storage::~Storage()
{
    mAllocator->dealloc(mData);
    free(mName);
}

void FrameLib_LocalAllocator::Storage::resize(unsigned long size)
{
    if (mMaxSize >= size && (size <= (mMaxSize >> 1)))
    {
        mSize = size;
    }
    else
    {
        double *newData = (double *) mAllocator->alloc(size * sizeof(double));
        
        if (newData)
        {
            mAllocator->dealloc(mData);
            mData = newData;
            mMaxSize = mSize = size;
        }
    }
}


// The Local Allocator

FrameLib_LocalAllocator::FrameLib_LocalAllocator(FrameLib_GlobalAllocator *allocator) : mGlobalAllocator(allocator)
{
    // Setup the free lists as a circularly linked list
    
    for (unsigned long i = 0; i < (numLocalFreeBlocks - 1); i++)
    {
        mFreeLists[i + 1].mPrev = mFreeLists + i;
        mFreeLists[i].mNext = mFreeLists + i + 1;
    }
    
    mTail = mFreeLists + (numLocalFreeBlocks - 1);
    mTail->mNext = mFreeLists;
    mTail->mNext->mPrev = mTail;
}

FrameLib_LocalAllocator::~FrameLib_LocalAllocator()
{
    clear();
}

void *FrameLib_LocalAllocator::alloc(size_t size)
{
    if (!size)
        return NULL;
    
    // N.B. - all memory should be aligned to alignment / memory returned will be between size and size << 1
    
    size_t maxSize = size << 1;
    
    // Find an appropriately sized block in the free lists
    
    for (FreeBlock *block = mTail->mNext; block && block->mMemory; block = block == mTail ? NULL : block->mNext)
        if (block->mSize >= size && block->mSize <= maxSize)
            return removeBlock(block);
    
    // If this fails call the global allocator
    
    return mGlobalAllocator->alloc(size);
}

void FrameLib_LocalAllocator::dealloc(void *ptr)
{
    if (ptr)
    {
        // If the free lists are full send the tail back to the global allocator
        
        if (mTail->mMemory)
            mGlobalAllocator->dealloc(mTail->mMemory);
        
        // Put the memory into the (now vacant) tail position
        
        mTail->mMemory = ptr;
        mTail->mSize = blockSize(ptr);
        
        // Move top and tail back one (old tail is new top)
        
        mTail = mTail->mPrev;
    }
}

void FrameLib_LocalAllocator::clear()
{
    // Acquire the main allocator and then free all blocks before releasing
    
    FrameLib_MainAllocator *allocator = mGlobalAllocator->acquire();
    
    for (unsigned long i = 0; i < numLocalFreeBlocks; i++)
    {
        if (mFreeLists[i].mMemory)
        {
            allocator->dealloc(mFreeLists[i].mMemory);
            mFreeLists[i].mMemory = NULL;
            mFreeLists[i].mSize = 0;
        }
    }
    
    mGlobalAllocator->release(&allocator);
}

size_t FrameLib_LocalAllocator::alignSize(size_t x)
{
    return alignedSize(x);
}

FrameLib_LocalAllocator::Storage *FrameLib_LocalAllocator::registerStorage(const char *name)
{
    std::vector<Storage *>::iterator it = findStorage(name);
    
    if (it == mStorage.end())
    {
        mStorage.push_back(new Storage(name, this));
        it = mStorage.end();
    }
    else
        (*it)->increment();
    
    return *it;
}

void FrameLib_LocalAllocator::releaseStorage(const char *name)
{
    std::vector<Storage *>::iterator it = findStorage(name);
    
    if (it != mStorage.end() && (*it)->decrement() <= 0)
        mStorage.erase(it);
}

std::vector<FrameLib_LocalAllocator::Storage *>::iterator FrameLib_LocalAllocator::findStorage(const char *name)
{
    for (std::vector<Storage *>::iterator it = mStorage.begin(); it != mStorage.end(); it++)
        if (!strcmp((*it)->getName(), name))
            return it;
    
    return mStorage.end();
}

void *FrameLib_LocalAllocator::removeBlock(FreeBlock *block)
{
    void *ptr = block->mMemory;
    block->mMemory = NULL;
    
    // If at the tail there is no need to do anything
    
    if (block == mTail)
        return ptr;
    
    // Join list
    
    block->mNext->mPrev = block->mPrev;
    block->mPrev->mNext = block->mNext;
    
    // Place at the tail
    
    block->mPrev = mTail;
    block->mNext = mTail->mNext;
    
    mTail->mNext->mPrev = block;
    mTail->mNext = block;
    
    mTail = block;
    
    return ptr;
}
