
#ifndef FRAMELIB_MEMORY_H
#define FRAMELIB_MEMORY_H

#include "tlsf.h"
#include "FrameLib_Threading.h"
#include <vector>

// FIX - do alignment improvements ?? (be better as part of allocator directly)
// FIX - threadsafety?
// FIX - expand the block and do free heuristics differently/for malloc (always free large blocks for instance)
// FIX - cleanup and checks
// FIX - free storage?

// The Main Allocator (has no threadsafety)

class FrameLib_MainAllocator
{
    static size_t const initialSize = 1024 * 1024 * 2;
    static size_t const growSize = 1024 * 1024 * 2;
    
    struct Pool
    {
        Pool(tlsf_t tlsf, size_t size);
        ~Pool();
    
        Pool *mNext;

    private:
        
        tlsf_t mTLSF;
        pool_t mTSLFPool;
        void *mMemory;
    };
    
public:
    
    FrameLib_MainAllocator();
    ~FrameLib_MainAllocator();
    
    void *alloc(size_t size);
    void dealloc(void *ptr);
    
private:
    
    void addPool(size_t size);
    
    tlsf_t mTLSF;
    Pool *mFirstPool;
    Pool *mLastPool;
    
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
