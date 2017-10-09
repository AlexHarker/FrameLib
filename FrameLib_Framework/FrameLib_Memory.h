
#ifndef FRAMELIB_MEMORY_H
#define FRAMELIB_MEMORY_H

#include "tlsf.h"
#include "FrameLib_Threading.h"

#include <vector>
#include <ctime>
#include <string>

// The Global Allocator (adds threadsafety to the CoreAllocator)

class FrameLib_GlobalAllocator
{

private:
    
    // The Core Allocator (has no threadsafety)
    
    class CoreAllocator
    {        
        // ************************************************************************************** //
        
        // Memory Pools
        
        struct Pool
        {
            Pool(void *mem, size_t size)  : mUsedRecently(true), mTime(0), mSize(size), mPrev(NULL), mNext(NULL), mMem(mem) {}
            
            bool isFree() { return tlsf_pool_is_free(mMem); }
            
            bool mUsedRecently;
            time_t mTime;
            size_t mSize;
            Pool *mPrev;
            Pool *mNext;
            void *mMem;
        };
        
        // ************************************************************************************** //
        
        // Thread for Allocating System Memory
        
        class NewThread : public DelegateThread
        {
            
        public:
            
            NewThread(CoreAllocator *allocator) : DelegateThread(Thread::kHighPriority), mAllocator(allocator) {}
            
        private:
            
            virtual void doTask() { mAllocator->addScheduledPool(); };
            
            CoreAllocator *mAllocator;
        };
        
        // ************************************************************************************** //
        
        // Thread for Freeing System Memory
        
        class FreeThread : public TriggerableThread
        {
            
        public:
            
            FreeThread(CoreAllocator *allocator) : TriggerableThread(Thread::kLowPriority), mAllocator(allocator) {}
            
        private:
            
            virtual void doTask() { mAllocator->destroyScheduledPool(); };
            
            CoreAllocator *mAllocator;
        };
        
        // ************************************************************************************** //
        
    public:
        
        CoreAllocator();
        ~CoreAllocator();
        
        // Allocate and deallocate memory (plus pruning)
        
        void *alloc(size_t size);
        void dealloc(void *ptr);
        
        void prune();
        
    private:
        
        // Get a Pool Class a tlsf pool_t
        
        Pool *getPool(pool_t pool);
        
        // Pool Helpers
        
        static Pool *createPool(size_t size);
        static void destroyPool(Pool *pool);
        void linkPool(Pool *pool);
        void unlinkPool(Pool *pool);
        void poolToTop(Pool *pool);
        void insertPool(Pool *pool);
        void removePool(Pool *pool);
        
        // Scheduled creation/deletion
        
        void addScheduledPool();
        void destroyScheduledPool();
        
        // Member Variables
        
        tlsf_t mTLSF;
        Pool *mPools;
        
        size_t mOSAllocated;
        size_t mAllocated;
        
        size_t mLastDisposedPoolSize;
        AtomicPtr<Pool> mScheduledNewPool;
        AtomicPtr<Pool> mScheduledDisposePool;
        NewThread mAllocThread;
        FreeThread mFreeThread;
    };
    
    // ************************************************************************************** //
    
public:

    // The Pruner uses RAII to lock the CoreAllocator allowing repeated deallocation followed by a prune with a single lock
    
    class Pruner
    {
        
    public:
        
        Pruner(FrameLib_GlobalAllocator *allocator) : mAllocator(allocator)
        {
            mAllocator->mLock.acquire();
        }
        
        ~Pruner()
        {
            mAllocator->mAllocator.prune();
            mAllocator->mLock.release();
        }
        
        void dealloc(void *ptr) { mAllocator->mAllocator.dealloc(ptr); }

    private:
        
        // Deleted
        
        Pruner(const Pruner&);
        Pruner& operator=(const Pruner&);
        
        // Allocator
        
        FrameLib_GlobalAllocator *mAllocator;
    };

    // ************************************************************************************** //

    // Constructor / Destructor
    
    FrameLib_GlobalAllocator() {}
    ~FrameLib_GlobalAllocator() {}
    
    // Allocate / Deallocate Memory
    
    void *alloc(size_t size);
    void dealloc(void *ptr);
    
    // Alignment Helpers
    
    static size_t getAlignment();
    static size_t alignSize(size_t x);
    
private:
    
    // Deleted
    
    FrameLib_GlobalAllocator(const FrameLib_GlobalAllocator&);
    FrameLib_GlobalAllocator& operator=(const FrameLib_GlobalAllocator&);
    
    // Member Variables
    
    SpinLock mLock;
    CoreAllocator mAllocator;
};

// ************************************************************************************** //

// The Local Allocator

class FrameLib_LocalAllocator
{
    // Local Blocks (free memory in double linked list (using internal pointers))
 
    static const int numLocalFreeBlocks = 16;

    struct FreeBlock
    {
        FreeBlock() : mMemory(NULL), mSize(0), mPrev(NULL), mNext(NULL) {}
        
        void *mMemory;
        size_t mSize;
        
        FreeBlock *mPrev;
        FreeBlock *mNext;
    };
    
    // ************************************************************************************** //
    
public:

    // Named Storage Local to Each Context
    
    class Storage
    {
        friend class FrameLib_LocalAllocator;

    public:
        
        // Getters
        
        double *getData() const         { return mData; }
        unsigned long getSize() const   { return mSize; }
        const char *getName() const     { return mName.c_str(); }
        
        // Resize the storage
        
        void resize(unsigned long size);

    protected:
        
        // Constructor / Destructor
        
        Storage(const char *name, FrameLib_LocalAllocator *allocator);
        ~Storage();
        
        // Reference Counting
        
        void increment()                { mCount++; }
        unsigned long decrement()       { return --mCount; }

    private:

        // Deleted
        
        Storage(const Storage&);
        Storage& operator=(const Storage&);
        
        // Member Variables
        
        std::string mName;
        
        double *mData;
        unsigned long mSize;
        unsigned long mMaxSize;
        unsigned long mCount;
        
        FrameLib_LocalAllocator *mAllocator;
    };
    
    typedef std::vector<Storage *>::iterator StorageIterator;

    // ************************************************************************************** //

    // Constructor / Destructor
    
    FrameLib_LocalAllocator(FrameLib_GlobalAllocator *allocator);
    ~FrameLib_LocalAllocator();
    
    // Allocate / Deallocate Memory

    void *alloc(size_t size);
    void dealloc(void *ptr);

    // Clear Local Free Blocks (and prune global allocator)
    
    void clear();
    
    // Alignment Helpers
    
    static size_t getAlignment()        { return FrameLib_GlobalAllocator::getAlignment(); }
    static size_t alignSize(size_t x)   { return FrameLib_GlobalAllocator::alignSize(x); }

    // Register and Release Storage
    
    Storage *registerStorage(const char *name);
    void releaseStorage(const char *name);
    
private:
    
    // Deleted
    
    FrameLib_LocalAllocator(const FrameLib_LocalAllocator&);
    FrameLib_LocalAllocator& operator=(const FrameLib_LocalAllocator&);
    
    // Find Storage by Name
    
    std::vector<Storage *>::iterator findStorage(const char *name);
    
    // Remove a Free Block after Allocation and Return the Pointer

    void *removeBlock(FreeBlock *block);
    
    // Member Variables
    
    FrameLib_GlobalAllocator *mAllocator;
    
    FreeBlock mFreeLists[numLocalFreeBlocks];
    FreeBlock *mTail;
    
    std::vector <Storage *> mStorage;
};

#endif
