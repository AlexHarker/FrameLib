
#include "FrameLib_Memory.h"
#include "FrameLib_Types.h"

// Utility

// FIX - THE SECOND OF THESE FUNCTIONS IS SLOW - cache mishit?

// N.B. - alignment must be a power of two

static const size_t alignment = 16;

size_t alignedSize(size_t x)    { return (x + (alignment - 1)) & ~(alignment - 1); }
size_t blockSize(void* ptr)     { return (*(size_t *) (((unsigned char*) ptr) - sizeof(size_t))) & ~(0x3); }


// Memory Pools

FrameLib_MainAllocator::Pool::Pool(tlsf_t tlsf, void *mem, size_t size) : mTLSF(tlsf), mTLSFPool(NULL), mUsedRecently(false), mTime(0), mSize(size), mPrev(NULL), mNext(NULL)
{
    mTLSFPool = tlsf_add_pool(mTLSF, mem, size);
}

FrameLib_MainAllocator::Pool::~Pool()
{
    if (mTLSFPool)
        tlsf_remove_pool(mTLSF, mTLSFPool);
}

bool FrameLib_MainAllocator::Pool::isFree()
{
    return tlsf_pool_is_free(mTLSFPool);
}

// The Main Allocator (has no threadsafety)

FrameLib_MainAllocator::FrameLib_MainAllocator() : mPools(NULL), mOSAllocated(0), mAllocated(0)
{
    mTLSF = tlsf_create(malloc(tlsf_size()));
    newPool(initialSize);
}

FrameLib_MainAllocator::~FrameLib_MainAllocator()
{
    while (mPools)
        deletePool(mPools);

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
        newPool(poolSize <= growSize ? growSize : poolSize);
        ptr = tlsf_memalign(mTLSF, alignment, size);
    }
    
    if (ptr)
        mAllocated += tlsf_block_size(ptr);
    
    return ptr;
}

void FrameLib_MainAllocator::dealloc(void *ptr)
{
    mAllocated -= tlsf_block_size(ptr);
    pool_t tlsfPool = tlsf_free(mTLSF, ptr);
    
    if (tlsfPool)
    {
        Pool *pool = getPool(tlsfPool);
        pool->mUsedRecently = true;
        removePool(pool);
        addPool(pool);
    }
}

// Pool Management

void FrameLib_MainAllocator::prune()
{
    Pool *pool = mPools->mPrev;
    time_t now;
    
    if (!pool->isFree())
    {
        removePool(pool);
        addPool(pool);
        return;
    }
    
    time(&now);
    
    if (pool->mUsedRecently)
    {
        pool->mUsedRecently = false;
        pool->mTime = now;
        return;
    }
    
    if (difftime(now, pool->mTime) > 30.0)
        deletePool(pool);
}

FrameLib_MainAllocator::Pool *FrameLib_MainAllocator::getPool(pool_t pool)
{
    return (Pool *) (((BytePointer) pool) - sizeof(Pool));
}

void FrameLib_MainAllocator::addPool(Pool *pool)
{
    if (!mPools)
    {
        pool->mPrev = pool;
        pool->mNext = pool;
    }
    else
    {
        pool->mPrev = mPools->mPrev;
        pool->mNext = mPools;
        mPools->mPrev = pool;
        pool->mPrev->mNext = pool;
    }
    
    mOSAllocated += pool->mSize;
    
    mPools = pool;
}

void FrameLib_MainAllocator::removePool(Pool *pool)
{
    pool->mPrev->mNext = pool->mNext;
    pool->mNext->mPrev = pool->mPrev;
    
    if (pool == mPools)
        mPools = pool->mNext == pool ? NULL : pool->mNext;
    
    mOSAllocated -= pool->mSize;
}

void FrameLib_MainAllocator::newPool(size_t size)
{
    // FIX - alignment for pool sizes
    
    // Allocate memory and create
    
    size_t actualSize = alignedSize(size) + alignedSize(tlsf_pool_overhead());
    void *memory = malloc(sizeof(Pool) + actualSize);
    
    Pool *pool = new(memory) Pool(mTLSF, ((BytePointer) memory) + sizeof(Pool), size);

    addPool(pool);
}

void FrameLib_MainAllocator::deletePool(Pool *pool)
{
    // Remove from the linked list
    
    removePool(pool);
    
    // Now free resources
    
    pool->~Pool();
    free(pool);
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

void FrameLib_GlobalAllocator::prune()
{
    mAllocator.prune();
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
    
    mGlobalAllocator->prune();
    mGlobalAllocator->release(&allocator);
}

size_t FrameLib_LocalAllocator::alignSize(size_t x)
{
    return alignedSize(x);
}

FrameLib_LocalAllocator::Storage *FrameLib_LocalAllocator::registerStorage(const char *name)
{
    std::vector<Storage *>::iterator it = findStorage(name);
    
    if (it != mStorage.end())
        return *it;
    
    mStorage.push_back(new Storage(name, this));
    return mStorage.back();
}

void FrameLib_LocalAllocator::releaseStorage(const char *name)
{
    std::vector<Storage *>::iterator it = findStorage(name);
    
    if (it != mStorage.end() && (*it)->decrement() <= 0)
    {
        delete *it;
        mStorage.erase(it);
    }
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
