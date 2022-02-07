
#include "FrameLib_Memory.h"
#include "FrameLib_Types.h"

// Static constants for memory scheduling and alignment

// N.B. - alignment must be a power of two

static constexpr size_t alignment = 16;
static size_t constexpr initSize = 1024 * 1024 * 2;
static size_t constexpr growSize = 1024 * 1024 * 2;
static constexpr int pruneInterval = 20;

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

FrameLib_GlobalAllocator::CoreAllocator::CoreAllocator(FrameLib_Thread::Priorities priorities, FrameLib_ErrorReporter& errorReporter) : mPools(nullptr), mOSAllocated(0), mAllocated(0), mLastDisposedPoolSize(0),  mScheduledNewPool(nullptr), mScheduledDisposePool(nullptr), mAllocThread(*this), mFreeThread(*this), mErrorReporter(errorReporter)
{
    mTLSF = tlsf_create(malloc(tlsf_size()));
    insertPool(createPool(initSize));
    
    mAllocThread.start(priorities);
    mFreeThread.start(priorities);
}

FrameLib_GlobalAllocator::CoreAllocator::~CoreAllocator()
{
    mAllocThread.join();
    mFreeThread.join();
    
    while (mPools)
    {
        Pool *pool = mPools;
        removePool(pool);
        destroyPool(pool);
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

        Pool *pool = nullptr;
        size_t poolSize = size <= (growSize >> 1) ? growSize : (size << 1);
        
        // Attempt to get the pool from the scheduled slot
        
        if (poolSize == growSize)
        {
            if (mAllocThread.completed())
                while (!(pool = mScheduledNewPool.exchange(nullptr)));
        }
        
        // If we still don't have pool try the disposed slot
        
        if (!pool && mLastDisposedPoolSize >= poolSize)
        {
            pool = mScheduledDisposePool.exchange(nullptr);
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
    else
        mErrorReporter(ErrorSource::Memory, nullptr, "FrameLib - couldn't allocate memory");
   
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
        
        if (difftime(now, pool->mTime) > pruneInterval && nullSwap(mScheduledDisposePool, pool))
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
    return (Pool *) (((BytePointer) pool) - alignedSize(sizeof(Pool)));
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
        mPools = pool->mNext == pool ? nullptr : pool->mNext;
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
    while (!nullSwap(mScheduledNewPool, pool));
}

void FrameLib_GlobalAllocator::CoreAllocator::destroyScheduledPool()
{
    destroyPool(mScheduledDisposePool.exchange(nullptr));
}

// ************************************************************************************** //

// The Global Allocator (adds threadsafety to the CoreAllocator)

// Allocate / Deallocate Memory

void *FrameLib_GlobalAllocator::alloc(size_t size)
{
    FrameLib_SpinLockHolder lock(&mLock);
    return mAllocator.alloc(size);
}

void FrameLib_GlobalAllocator::dealloc(void *ptr)
{
    FrameLib_SpinLockHolder lock(&mLock);
    mAllocator.dealloc(ptr);
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

// Context Local Storage

FrameLib_ContextAllocator::Storage::Storage(const char *name, FrameLib_ContextAllocator& allocator)
:  mName(name), mType(FrameType::Vector), mData(nullptr), mSize(0), mMaxSize(0), mCount(1), mAllocator(allocator)
{}

FrameLib_ContextAllocator::Storage::~Storage()
{
    if (mType == FrameType::Tagged)
        getTagged()->~Serial();

    mAllocator.dealloc(mData);
}

void FrameLib_ContextAllocator::Storage::resize(bool tagged, unsigned long size)
{
    size_t actualSize = tagged ? Serial::inPlaceSize(size) : size * sizeof(double);
    size_t maxSize = actualSize << 1;

    if (mMaxSize >= maxSize)
    {
        // Reallocate for tagged frames
        
        if (mType == FrameType::Tagged)
            getTagged()->~Serial();
        if (tagged)
            Serial::newInPlace(mData, size);
        
        // Set Parameters
        
        mType = tagged ? FrameType::Tagged : FrameType::Vector;
        mSize = size;
    }
    else
    {   
        void *newData = mAllocator.alloc(maxSize);
        
        if (newData)
        {
            // Deallocate
            
            if (mType == FrameType::Tagged)
                getTagged()->~Serial();
            mAllocator.dealloc(mData);
            
            // Allocate
            
            mData = newData;
            if (tagged)
                Serial::newInPlace(newData, size);
            
            // Set parameters
            
            mType = tagged ? FrameType::Tagged : FrameType::Vector;
            mMaxSize = maxSize;
            mSize = size;
        }
    }
}

// Context Local Storage Pointer

void FrameLib_ContextAllocator::StoragePtr::release()
{
    if (!mStorage)
        return;
    
    if (mStorage->decrement() <= 0)
    {
        for (auto it = mAllocator->mStorage.begin(); it != mAllocator->mStorage.end(); it++)
        {
            if (*it == mStorage)
            {
                mAllocator->mStorage.erase(it);
                break;
            }
        }
        
        delete mStorage;
    }
    
    mStorage = nullptr;
}

// ************************************************************************************** //

// Thread Local Allocator

// Constructor / Destructor

FrameLib_LocalAllocator::FrameLib_LocalAllocator(FrameLib_GlobalAllocator& allocator)
: mAllocator(allocator)
{
    // Setup the free lists as a circularly linked list
    
    for (unsigned int i = 0; i < (numLocalFreeBlocks - 1); i++)
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
        return nullptr;
    
    // N.B. - all memory should be aligned to alignment / memory returned will be between size and size << 1
    
    size_t maxSize = size << 1;
    
    // Find an appropriately sized block in the free lists
    
    for (FreeBlock *block = mTail->mNext; block && block->mMemory; block = block == mTail ? nullptr : block->mNext)
        if (block->mSize >= size && block->mSize <= maxSize)
            return removeBlock(block);
     
    // If this fails call the global allocator

    return mAllocator.alloc(size);
}

void FrameLib_LocalAllocator::dealloc(void *ptr)
{
    if (ptr)
    {
        // If the free lists are full send the tail back to the global allocator
        
        if (mTail->mMemory)
            mAllocator.dealloc(mTail->mMemory);
        
        // Put the memory into the (now vacant) tail position
        
        mTail->mMemory = ptr;
        mTail->mSize = blockSize(ptr);
        
        // Move top and tail back one (old tail is now the top)
        
        mTail = mTail->mPrev;
    }
}

// Remove a Free Block after Allocation and Return the Pointer

void *FrameLib_LocalAllocator::removeBlock(FreeBlock *block)
{
    void *ptr = block->mMemory;
    
    // Set to default values
    
    block->mMemory = nullptr;
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

void FrameLib_LocalAllocator::clear()
{
    // Acquire the main allocator and then free all blocks before releasing
    
    FrameLib_GlobalAllocator::Pruner pruner(mAllocator);
    
    for (unsigned int i = 0; i < numLocalFreeBlocks; i++)
    {
        if (mFreeLists[i].mMemory)
        {
            pruner.dealloc(mFreeLists[i].mMemory);
            mFreeLists[i].mMemory = nullptr;
            mFreeLists[i].mSize = 0;
        }
     }
}

// ************************************************************************************** //

// Thread Local Allocator Set

FrameLib_LocalAllocatorSet::FrameLib_LocalAllocatorSet(FrameLib_GlobalAllocator& allocator, unsigned int size)
{
    for (unsigned int i = 0; i < size; i++)
        mAllocators.add(new FrameLib_LocalAllocator(allocator));
}

void FrameLib_LocalAllocatorSet::clear()
{
    // Acquire the main allocator and then free all blocks before releasing
    
    FrameLib_GlobalAllocator::Pruner pruner(mAllocators[0]->mAllocator);
    
    for (unsigned int i = 0; i < mAllocators.size(); i++)
    {
        FrameLib_LocalAllocator *allocator = get(i);
        
        for (unsigned int j = 0; j < FrameLib_LocalAllocator::numLocalFreeBlocks; j++)
        {
            if (allocator->mFreeLists[j].mMemory)
            {
                pruner.dealloc(allocator->mFreeLists[j].mMemory);
                allocator->mFreeLists[j].mMemory = nullptr;
                allocator->mFreeLists[j].mSize = 0;
            }
        }
    }
}

// ************************************************************************************** //

// The Context Allocator

// Allocate / Deallocate Memory

void *FrameLib_ContextAllocator::alloc(size_t size)
{
    if (!size)
        return nullptr;
    
    return mAllocator.alloc(size);
}

void FrameLib_ContextAllocator::dealloc(void *ptr)
{
    // Deallocate using free blocks if present
    
    if (ptr)
        mAllocator.dealloc(ptr);
}

// Prune the global allocator

void FrameLib_ContextAllocator::prune()
{
    // Prune the global allocator
    
    FrameLib_GlobalAllocator::Pruner pruner(mAllocator);
}

// Register Storage

FrameLib_ContextAllocator::StoragePtr FrameLib_ContextAllocator::registerStorage(const char *name)
{
    auto it = findStorage(name);
    
    if (it != mStorage.end())
    {
        (*it)->increment();
        return StoragePtr(this, *it);
    }
    
    mStorage.push_back(new Storage(name, *this));
    return StoragePtr(this, mStorage.back());
}

// Find Storage by Name

std::vector<FrameLib_ContextAllocator::Storage *>::iterator FrameLib_ContextAllocator::findStorage(const char *name)
{
    for (auto it = mStorage.begin(); it != mStorage.end(); it++)
        if (!strcmp((*it)->getName(), name))
            return it;
    
    return mStorage.end();
}
