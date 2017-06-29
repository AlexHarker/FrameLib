
#include "FrameLib_Memory.h"
#include "FrameLib_Types.h"

// Static constants for memory scheduling and alignment

// N.B. - alignment must be a power of two

static const size_t alignment = 16;
static size_t const initSize = 1024 * 1024 * 2;
static size_t const growSize = 1024 * 1024 * 2;
static const int pruneInterval = 20;


// Utility

size_t alignedSize(size_t x)
{
    return (x + (alignment - 1)) & ~(alignment - 1);
}

// FIX - THIS IS SLOW AND NOT INLINE - cahce mishit/extra function call - test this

inline size_t blockSize(void* ptr)
{
    return tlsf_block_size(ptr);
}


// The Main Allocator (has no threadsafety)

FrameLib_MainAllocator::FrameLib_MainAllocator() : mPools(NULL), mOSAllocated(0), mAllocated(0), lastDisposedPoolSize(0), allocThread(this), freeThread(this)
{
    mTLSF = tlsf_create(malloc(tlsf_size()));
    insertPool(createPool(initSize));
}

FrameLib_MainAllocator::~FrameLib_MainAllocator()
{
    while (mPools)
    {
        removePool(mPools);
        destroyPool(mPools);
    }

    tlsf_destroy(mTLSF);
    free(mTLSF);
}

void *FrameLib_MainAllocator::alloc(size_t size)
{
    void *ptr = tlsf_memalign(mTLSF, alignment, size);
    
    // If the allocation fails we need to add a new memory pool
    
    if (!ptr)
    {
        // FIX - for now allocate double the necessary size (which will always work)

        Pool *pool = NULL;
        size_t poolSize = size <= (growSize >> 1) ? growSize : (size << 1);
        
        // Attempt to get a new pool from the scheduled slot
        
        if (poolSize == growSize)
        {
            if (allocThread.completed())
                while (!(pool = scheduledNewPool.clear()));
        }
        
        // If we still don't have pool try the disposed slot
        
        if (!pool && lastDisposedPoolSize >= poolSize)
        {
            pool = scheduledDisposePool.clear();
            lastDisposedPoolSize = 0;
        }

        // Finally try to create one in this thread
        
        if (!pool)
            pool = createPool(poolSize);
        
        // Insert the pool and attempt to allocate
        
        insertPool(pool);
        ptr = tlsf_memalign(mTLSF, alignment, size);
    }
    
    // Update the allocated size
    
    if (ptr)
        mAllocated += blockSize(ptr);
    
    // Check for near full
    
    if (mOSAllocated < mAllocated + growSize)
        allocThread.signal();
        
    return ptr;
}

void FrameLib_MainAllocator::dealloc(void *ptr)
{
    mAllocated -= blockSize(ptr);
    pool_t tlsfPool = tlsf_free(mTLSF, ptr);
    
    if (tlsfPool)
        poolToTop(getPool(tlsfPool));
}

// Pool Management

void FrameLib_MainAllocator::prune()
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
        
        if (difftime(now, pool->mTime) > pruneInterval && scheduledDisposePool.compareAndSwap(NULL, pool))
        {
            removePool(pool);
            lastDisposedPoolSize = pool->mSize;
            freeThread.signal();
        }
    }
}

// Get a Pool class from the tlsf pool_t

FrameLib_MainAllocator::Pool *FrameLib_MainAllocator::getPool(pool_t pool)
{
    return (Pool *) (((BytePointer) pool) - sizeof(Pool));
}

// Pool Helpers

FrameLib_MainAllocator::Pool *FrameLib_MainAllocator::createPool(size_t size)
{
    // FIX - alignment for pool sizes
    
    void *memory = malloc(sizeof(Pool) + alignedSize(size) + alignedSize(tlsf_pool_overhead()));
    
    return new(memory) Pool(((BytePointer) memory) + sizeof(Pool), size);
}

void FrameLib_MainAllocator::destroyPool(Pool *pool)
{
    if (pool)
    {
        pool->~Pool();
        free(pool);
    }
}

void FrameLib_MainAllocator::linkPool(Pool *pool)
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

void FrameLib_MainAllocator::unlinkPool(Pool *pool)
{
    pool->mPrev->mNext = pool->mNext;
    pool->mNext->mPrev = pool->mPrev;
    
    if (pool == mPools)
        mPools = pool->mNext == pool ? NULL : pool->mNext;
}

void FrameLib_MainAllocator::poolToTop(Pool *pool)
{
    pool->mUsedRecently = true;
    unlinkPool(pool);
    linkPool(pool);
}

void FrameLib_MainAllocator::insertPool(Pool *pool)
{
    tlsf_add_pool(mTLSF, pool->mMem, pool->mSize);
    linkPool(pool);
    mOSAllocated += pool->mSize;
}

void FrameLib_MainAllocator::removePool(Pool *pool)
{
    tlsf_remove_pool(mTLSF, pool->mMem);
    unlinkPool(pool);
    mOSAllocated -= pool->mSize;
}

// Scheduled creation/deletion

void FrameLib_MainAllocator::addScheduledPool()
{
    Pool *pool = createPool(growSize);
    while (!scheduledNewPool.compareAndSwap(NULL, pool));
}

void FrameLib_MainAllocator::destroyScheduledPool()
{
    destroyPool(scheduledDisposePool.clear());
}


// The global allocator (adds threadsaftey to the main allocator)

// Acquire/release the internal allocator directly

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

// Allocate or deallocate memory

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

// Prune available memory (examine usage and free appropriate unsed pools)

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

// Constructor / Destructor

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

// Allocate or deallocate memory

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

// Clear local free blocks (and call to prune global allocator)

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

// Return new size after adjustment for alignment

size_t FrameLib_LocalAllocator::alignSize(size_t x)
{
    return alignedSize(x);
}

// Register and release storage

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

// Find storage by name

std::vector<FrameLib_LocalAllocator::Storage *>::iterator FrameLib_LocalAllocator::findStorage(const char *name)
{
    for (std::vector<Storage *>::iterator it = mStorage.begin(); it != mStorage.end(); it++)
        if (!strcmp((*it)->getName(), name))
            return it;
    
    return mStorage.end();
}

// Remove a free block after allocation and return the pointer

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
