
#ifndef FRAMELIB_MEMORY_H
#define FRAMELIB_MEMORY_H

#include "tlsf.h"
#include "FrameLib_Threading.h"

#include <vector>
#include <ctime>

// FIX - do alignment improvements ?? (be better as part of allocator directly)
// FIX - threadsafety?
// FIX - expand the block and do free heuristics differently/for malloc (always free large blocks for instance)
// FIX - cleanup and checks
// FIX - free storage once it is no longer referenced? or when allocator is destroyed?

// The Main Allocator (has no threadsafety)

class FrameLib_MainAllocator
{
    static size_t const initialSize = 1024 * 1024 * 2;
    static size_t const growSize = 1024 * 1024 * 2;
    
    struct Pool
    {
        Pool(tlsf_t tlsf, void *mem, size_t size);
        ~Pool();
    
        bool isFree();
        
    private:
        
        tlsf_t mTLSF;
        pool_t mTLSFPool;
        
    public:
        
        bool mUsedRecently;
        time_t mTime;
        size_t mSize;
        Pool *mPrev;
        Pool *mNext;
    };
    
public:
    
    FrameLib_MainAllocator();
    ~FrameLib_MainAllocator();
    
    void *alloc(size_t size);
    void dealloc(void *ptr);
    
    void prune();
    
private:
    
    Pool *getPool(pool_t pool);
    void addPool(Pool *pool);
    void removePool(Pool *pool);
    void newPool(size_t size);
    void deletePool(Pool *pool);

    tlsf_t mTLSF;
    Pool *mPools;
    
    size_t mOSAllocated;
    size_t mAllocated;
};


// The Global Allocator (adds threadsaftey to the main allocator)

class FrameLib_GlobalAllocator
{

public:

    FrameLib_MainAllocator *acquire();
    bool release(FrameLib_MainAllocator **allocator);

    void *alloc(size_t size);
    void dealloc(void *ptr);
    
    void prune();
    
private:
    
    FrameLib_SpinLock mLock;
    FrameLib_MainAllocator mAllocator;

};

// The Local Allocator

class FrameLib_LocalAllocator
{
    static const int numLocalFreeBlocks = 16;
    
    struct FreeBlock
    {
        FreeBlock() : mMemory(NULL), mSize(0), mPrev(NULL), mNext(NULL) {}
        
        void *mMemory;
        size_t mSize;
        
        FreeBlock *mPrev;
        FreeBlock *mNext;
    };

public:
    
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
        
        double *mData;
        unsigned long mSize;
        unsigned long mMaxSize;
        unsigned long mCount;
        char *mName;
        
        FrameLib_LocalAllocator *mAllocator;
    };
    
public:

    FrameLib_LocalAllocator(FrameLib_GlobalAllocator *allocator);
    ~FrameLib_LocalAllocator();
    
    void *alloc(size_t size);
    void dealloc(void *ptr);
    
    void clear();
    
    static size_t alignSize(size_t x);
    
    Storage *registerStorage(const char *name);
    void releaseStorage(const char *name);
    
private:
    
    std::vector<Storage *>::iterator findStorage(const char *name);
    
    void *removeBlock(FreeBlock *block);
    
    FrameLib_GlobalAllocator *mGlobalAllocator;
    
    FreeBlock mFreeLists[numLocalFreeBlocks];
    FreeBlock *mTail;
    
    std::vector <Storage *> mStorage;
};

#endif
