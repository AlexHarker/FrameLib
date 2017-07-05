
#include "FrameLib_Memory.h"
#include "FrameLib_Types.h"

// Static constants for memory scheduling and alignment

// N.B. - alignment must be a power of two

static const size_t alignment = 16;
static size_t const initSize = 1024 * 1024 * 2;
static size_t const growSize = 1024 * 1024 * 2;
static const int pruneInterval = 20;

// ************************************************************************************** //

// Utility

size_t alignedSize(size_t x)
{
    return (x + (alignment - 1)) & ~(alignment - 1);
}

inline size_t blockSize(void* ptr)
{
    return tlsf_block_size(ptr);
}

// ************************************************************************************** //

// The Core Allocator (has no threadsafety)

FrameLib_GlobalAllocator::CoreAllocator::CoreAllocator() : mPools(NULL), mOSAllocated(0), mAllocated(0), mLastDisposedPoolSize(0), mAllocThread(this), mFreeThread(this)
{
    mTLSF = tlsf_create(malloc(tlsf_size()));
    insertPool(createPool(initSize));
    
    mAllocThread.start();
    mFreeThread.start();
}

FrameLib_GlobalAllocator::CoreAllocator::~CoreAllocator()
{
    mAllocThread.join();
    mFreeThread.join();
    
    while (mPools)
    {
        removePool(mPools);
        destroyPool(mPools);
    }

    tlsf_destroy(mTLSF);
    free(mTLSF);
}

void *FrameLib_GlobalAllocator::CoreAllocator::alloc(size_t size)
{
    void *ptr = tlsf_memalign(mTLSF, alignment, size);
    
    // If the allocation fails we need to add another memory pool
    
    if (!ptr)
    {
        // N.B. - for now allocate double the necessary size (which should always work)

        Pool *pool = NULL;
        size_t poolSize = size <= (growSize >> 1) ? growSize : (size << 1);
        
        // Attempt to get the pool from the scheduled slot
        
        if (poolSize == growSize)
        {
            if (mAllocThread.completed())
                while (!(pool = mScheduledNewPool.clear()));
        }
        
        // If we still don't have pool try the disposed slot
        
        if (!pool && mLastDisposedPoolSize >= poolSize)
        {
            pool = mScheduledDisposePool.clear();
            mLastDisposedPoolSize = 0;
        }

        // Finally try to create one in this thread
        
        if (!pool)
            pool = createPool(poolSize);
        
        // Insert the pool and attempt to allocate
        
        if (pool)
            insertPool(pool);
        
        ptr = tlsf_memalign(mTLSF, alignment, size);
    }
    
    // Update the allocated size
    
    if (ptr)
        mAllocated += blockSize(ptr);
    
    // Check for near full
    
    if (mOSAllocated < mAllocated + growSize)
        mAllocThread.signal();
        
    return ptr;
}

void FrameLib_GlobalAllocator::CoreAllocator::dealloc(void *ptr)
{
    mAllocated -= blockSize(ptr);
    pool_t tlsfPool = tlsf_free(mTLSF, ptr);
    
    if (tlsfPool)
        poolToTop(getPool(tlsfPool));
}

// Pool Management

void FrameLib_GlobalAllocator::CoreAllocator::prune()
{
    if (mPools)
    {
        Pool *pool = mPools->mPrev;
    
        if (!pool->isFree())
        {
            poolToTop(pool);
            return;
        }
        
        time_t now;
        time(&now);
        
        if (pool->mUsedRecently)
        {
            pool->mUsedRecently = false;
            pool->mTime = now;
            return;
        }
        
        if (difftime(now, pool->mTime) > pruneInterval && mScheduledDisposePool.compareAndSwap(NULL, pool))
        {
            removePool(pool);
            mLastDisposedPoolSize = pool->mSize;
            mFreeThread.signal();
        }
    }
}

// Get a Pool Class From a tlsf pool_t

FrameLib_GlobalAllocator::CoreAllocator::Pool *FrameLib_GlobalAllocator::CoreAllocator::getPool(pool_t pool)
{
    return (Pool *) (((BytePointer) pool) - sizeof(Pool));
}

// Pool Helpers

FrameLib_GlobalAllocator::CoreAllocator::Pool *FrameLib_GlobalAllocator::CoreAllocator::createPool(size_t size)
{    
    void *memory = malloc(alignedSize(sizeof(Pool)) + alignedSize(size) + alignedSize(tlsf_pool_overhead()));
    
    return new(memory) Pool(((BytePointer) memory) + alignedSize(sizeof(Pool)), size);
}

void FrameLib_GlobalAllocator::CoreAllocator::destroyPool(Pool *pool)
{
    if (pool)
    {
        pool->~Pool();
        free(pool);
    }
}

void FrameLib_GlobalAllocator::CoreAllocator::linkPool(Pool *pool)
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
    
    mPools = pool;
}

void FrameLib_GlobalAllocator::CoreAllocator::unlinkPool(Pool *pool)
{
    pool->mPrev->mNext = pool->mNext;
    pool->mNext->mPrev = pool->mPrev;
    
    if (pool == mPools)
        mPools = pool->mNext == pool ? NULL : pool->mNext;
}

void FrameLib_GlobalAllocator::CoreAllocator::poolToTop(Pool *pool)
{
    pool->mUsedRecently = true;
    unlinkPool(pool);
    linkPool(pool);
}

void FrameLib_GlobalAllocator::CoreAllocator::insertPool(Pool *pool)
{
    tlsf_add_pool(mTLSF, pool->mMem, pool->mSize);
    linkPool(pool);
    mOSAllocated += pool->mSize;
}

void FrameLib_GlobalAllocator::CoreAllocator::removePool(Pool *pool)
{
    tlsf_remove_pool(mTLSF, pool->mMem);
    unlinkPool(pool);
    mOSAllocated -= pool->mSize;
}

// Scheduled creation/deletion

void FrameLib_GlobalAllocator::CoreAllocator::addScheduledPool()
{
    Pool *pool = createPool(growSize);
    while (!mScheduledNewPool.compareAndSwap(NULL, pool));
}

void FrameLib_GlobalAllocator::CoreAllocator::destroyScheduledPool()
{
    destroyPool(mScheduledDisposePool.clear());
}

// ************************************************************************************** //

// The Global Allocator (adds threadsafety to the CoreAllocator)

// Allocate / Deallocate Memory

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

// Alignment Helpers

size_t FrameLib_GlobalAllocator::getAlignment()
{
    return alignment;
}

size_t FrameLib_GlobalAllocator::alignSize(size_t x)
{
    return alignedSize(x);
}

// ************************************************************************************** //

// Local Storage

FrameLib_LocalAllocator::Storage::Storage(const char *name, FrameLib_LocalAllocator *allocator)
:  mName(name), mData(NULL), mSize(0), mMaxSize(0), mCount(1), mAllocator(allocator)
{}

FrameLib_LocalAllocator::Storage::~Storage()
{
    mAllocator->dealloc(mData);
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

// ************************************************************************************** //

// The Local Allocator

// Constructor / Destructor

FrameLib_LocalAllocator::FrameLib_LocalAllocator(FrameLib_GlobalAllocator *allocator) : mAllocator(allocator)
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

// Allocate / Deallocate Memory

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
    
    return mAllocator->alloc(size);
}

void FrameLib_LocalAllocator::dealloc(void *ptr)
{
    if (ptr)
    {
        // If the free lists are full send the tail back to the global allocator
        
        if (mTail->mMemory)
            mAllocator->dealloc(mTail->mMemory);
        
        // Put the memory into the (now vacant) tail position
        
        mTail->mMemory = ptr;
        mTail->mSize = blockSize(ptr);
        
        // Move top and tail back one (old tail is now the top)
        
        mTail = mTail->mPrev;
    }
}

// Clear Local Free Blocks (and prune global allocator)

void FrameLib_LocalAllocator::clear()
{
    // Acquire the main allocator and then free all blocks before releasing
    
    FrameLib_GlobalAllocator::Pruner pruner(mAllocator);
    
    for (unsigned long i = 0; i < numLocalFreeBlocks; i++)
    {
        if (mFreeLists[i].mMemory)
        {
            pruner.dealloc(mFreeLists[i].mMemory);
            mFreeLists[i].mMemory = NULL;
            mFreeLists[i].mSize = 0;
        }
    }
}

// Register and Release Storage

FrameLib_LocalAllocator::Storage *FrameLib_LocalAllocator::registerStorage(const char *name)
{
    std::vector<Storage *>::iterator it = findStorage(name);
    
    if (it != mStorage.end())
    {
        (*it)->increment();
        return *it;
    }
    
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

// Find Storage by Name

std::vector<FrameLib_LocalAllocator::Storage *>::iterator FrameLib_LocalAllocator::findStorage(const char *name)
{
    for (std::vector<Storage *>::iterator it = mStorage.begin(); it != mStorage.end(); it++)
        if (!strcmp((*it)->getName(), name))
            return it;
    
    return mStorage.end();
}

// Remove a Free Block after Allocation and Return the Pointer

void *FrameLib_LocalAllocator::removeBlock(FreeBlock *block)
{
    void *ptr = block->mMemory;
    
    // Set to default values
    
    block->mMemory = NULL;
    block->mSize = 0;
    
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
