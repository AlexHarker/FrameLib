
#ifndef FRAMELIB_MEMORY_H
#define FRAMELIB_MEMORY_H

#include "tlsf.h"
#include "FrameLib_Threading.h"

#include <vector>
#include <ctime>

// FIX - do alignment improvements ?? (be better as part of allocator directly)
// FIX - expand the block and do free heuristics differently/for malloc (always free large blocks for instance)

// The Main Allocator (has no threadsafety)

class FrameLib_MainAllocator
{    
    struct Pool
    {
        Pool(void *mem, size_t size);
    
        bool isFree();
        
        bool mUsedRecently;
        time_t mTime;
        size_t mSize;
        Pool *mPrev;
        Pool *mNext;
        void *mMem;
    };
    
    struct NewThread : public SyncThread
    {
        NewThread(FrameLib_MainAllocator *allocator) : mAllocator(allocator){}
        
    private:
        
        virtual void doTask();
        
        FrameLib_MainAllocator *mAllocator;
    };
    
    struct FreeThread : public SignalableThread
    {
        FreeThread(FrameLib_MainAllocator *allocator) : mAllocator(allocator){}
        
    private:
        
        virtual void doTask();
        
        FrameLib_MainAllocator *mAllocator;
    };
    
public:
    
    FrameLib_MainAllocator();
    ~FrameLib_MainAllocator();
    
    void *alloc(size_t size);
    void dealloc(void *ptr);
    
    void prune();
    
    void addScheduledPool(Pool *pool) { while (!scheduledNewPool.compareAndSwap(NULL, pool)); }
    Pool *getScheduledPool() { return scheduledDisposePool.clear(); }
    
private:
    
    Pool *getPool(pool_t pool);
    
    static Pool *createPool(size_t size);
    static void destroyPool(Pool *pool);
    void linkPool(Pool *pool);
    void unlinkPool(Pool *pool);
    void poolToTop(Pool *pool);
    void insertPool(Pool *pool);
    void removePool(Pool *pool);

    // TSLF allocator and pools
    tlsf_t mTLSF;
    Pool *mPools;
    
    size_t mOSAllocated;
    size_t mAllocated;
    
    size_t lastDisposedPoolSize;
    FrameLib_AtomicPtr<Pool> scheduledNewPool;
    FrameLib_AtomicPtr<Pool> scheduledDisposePool;
    NewThread allocThread;
    FreeThread freeThread;
};


// The global allocator (simply adds threadsaftey to the main allocator)

class FrameLib_GlobalAllocator
{

public:

    // Acquire/release the internal allocator directly
    
    FrameLib_MainAllocator *acquire();
    bool release(FrameLib_MainAllocator **allocator);

    // Allocate or deallocate memory
    
    void *alloc(size_t size);
    void dealloc(void *ptr);
    
    // Prune available memory (examine usage and free appropriate unsed pools)
    
    void prune();
    
private:
    
    FrameLib_SpinLock mLock;
    FrameLib_MainAllocator mAllocator;

};


// The local allocator

class FrameLib_LocalAllocator
{
    static const int numLocalFreeBlocks = 16;
    
    // Local blocks of free memory in double linked list (using internal pointers)
    
    struct FreeBlock
    {
        FreeBlock() : mMemory(NULL), mSize(0), mPrev(NULL), mNext(NULL) {}
        
        void *mMemory;
        size_t mSize;
        
        FreeBlock *mPrev;
        FreeBlock *mNext;
    };

public:
    
    // Named storage local to each context
    
    // FIX - style
    
    struct Storage
    {
        friend FrameLib_LocalAllocator;

    protected:
        
        Storage(const char *name, FrameLib_LocalAllocator *allocator);
        ~Storage();
        
        void increment()            { mCount++; }
        unsigned long decrement()   { return --mCount; }
        
    public:
        
        double *getData() const           { return mData; }
        unsigned long getSize() const     { return mSize; }
        const char *getName() const       { return mName; }
        
        void resize(unsigned long size);
        
    private:

        char *mName;
        
        double *mData;
        unsigned long mSize;
        unsigned long mMaxSize;
        unsigned long mCount;
        
        FrameLib_LocalAllocator *mAllocator;
    };
    
    // Constructor / Destructor
    
    FrameLib_LocalAllocator(FrameLib_GlobalAllocator *allocator);
    ~FrameLib_LocalAllocator();
    
    // Allocate or deallocate memory

    void *alloc(size_t size);
    void dealloc(void *ptr);

    // Clear local free blocks (and call to prune global allocator)
    
    void clear();
    
    // Return new size after adjustment for alignment
    
    static size_t alignSize(size_t x);
    
    // Register and release storage
    
    Storage *registerStorage(const char *name);
    void releaseStorage(const char *name);
    
private:
    
    // Find storage by name
    
    std::vector<Storage *>::iterator findStorage(const char *name);
    
    // Remove a free block after allocation and return the pointer

    void *removeBlock(FreeBlock *block);
    
    // Pointer to global allocator, blocks and storage
    
    FrameLib_GlobalAllocator *mGlobalAllocator;
    
    FreeBlock mFreeLists[numLocalFreeBlocks];
    FreeBlock *mTail;
    
    std::vector <Storage *> mStorage;
};

#endif
