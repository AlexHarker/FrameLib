
#ifndef FRAMELIB_MEMORY_H
#define FRAMELIB_MEMORY_H

#include "../FrameLib_Dependencies/tlsf/tlsf.h"
#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"
#include "FrameLib_Parameters.h"
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
            Pool(void *mem, size_t size)  : mUsedRecently(true), mTime(0), mSize(size), mPrev(nullptr), mNext(nullptr), mMem(mem) {}
            
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
        
        class NewThread final : public DelegateThread
        {
            
        public:
            
            NewThread(CoreAllocator *allocator) : DelegateThread(Thread::kHighPriority), mAllocator(allocator) {}
            
        private:
            
            void doTask() override { mAllocator->addScheduledPool(); };
            
            CoreAllocator *mAllocator;
        };
        
        // ************************************************************************************** //
        
        // Thread for Freeing System Memory
        
        class FreeThread final : public TriggerableThread
        {
            
        public:
            
            FreeThread(CoreAllocator *allocator) : TriggerableThread(Thread::kLowPriority), mAllocator(allocator) {}
            
        private:
            
            void doTask() override { mAllocator->destroyScheduledPool(); };
            
            CoreAllocator *mAllocator;
        };
        
        // ************************************************************************************** //
        
    public:
        
        CoreAllocator(FrameLib_ErrorReporter& errorReporter);
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
        std::atomic<Pool *> mScheduledNewPool;
        std::atomic<Pool *> mScheduledDisposePool;
        NewThread mAllocThread;
        FreeThread mFreeThread;
        
        FrameLib_ErrorReporter& mErrorReporter;
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
        
        Pruner(const Pruner&) = delete;
        Pruner& operator=(const Pruner&) = delete;
        
        // Allocator
        
        FrameLib_GlobalAllocator *mAllocator;
    };

    // ************************************************************************************** //

    // Constructor / Destructor
    
    FrameLib_GlobalAllocator(FrameLib_ErrorReporter& errorReporter) : mAllocator(errorReporter) {}
    ~FrameLib_GlobalAllocator() {}
    
    // Allocate / Deallocate Memory
    
    void *alloc(size_t size);
    void dealloc(void *ptr);
    
    // Alignment Helpers
    
    static size_t getAlignment();
    static size_t alignSize(size_t x);
    
private:
    
    // Deleted
    
    FrameLib_GlobalAllocator(const FrameLib_GlobalAllocator&) = delete;
    FrameLib_GlobalAllocator& operator=(const FrameLib_GlobalAllocator&) = delete;
    
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
        FreeBlock() : mMemory(nullptr), mSize(0), mPrev(nullptr), mNext(nullptr) {}
        
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
        typedef FrameLib_Parameters::Serial Serial;
        friend class FrameLib_LocalAllocator;

    public:
        
        // The Access Class Enforces Thread Safety for Storage
        
        class Access
        {
            
        public:
            
            // Constructor and Destructor
            
            Access(Storage *storage) : mStorage(storage)    { mStorage->mLock.acquire(); }
            ~Access()                                       { mStorage->mLock.release(); }
            
            // Getters
            
            FrameType getType() const               { return mStorage->getType(); }
            double *getVector() const               { return mStorage->getVector(); }
            unsigned long getVectorSize() const     { return mStorage->getVectorSize(); }
            unsigned long getTaggedSize() const     { return mStorage->getTaggedSize(); }
            Serial *getTagged() const               { return mStorage->getTagged(); }
            
            // Resize
            
            void resize(bool tagged, size_t size)   { mStorage->resize(tagged, size); }
            
        private:
            
            // Deleted
            
            Access(const Access&) = delete;
            Access& operator=(const Access&) = delete;
            
            // Data
            
            Storage *mStorage;
        };
      
        const char *getName() const             { return mName.c_str(); }
        
    protected:

        // Getters
        
        FrameType getType() const               { return mType; }
        double *getVector() const               { return mType == kFrameNormal ? static_cast<double *>(mData) : nullptr; }
        unsigned long getVectorSize() const     { return mType == kFrameNormal ? mSize : 0; }
        unsigned long getTaggedSize() const     { return mType == kFrameTagged ? mSize : 0; }
        Serial *getTagged() const               { return mType == kFrameTagged ? static_cast<Serial *>(mData) : nullptr; }
        
        // Resize the storage
        
        void resize(bool tagged, size_t size);

        // Constructor / Destructor
        
        Storage(const char *name, FrameLib_LocalAllocator *allocator);
        ~Storage();
        
        // Reference Counting
        
        void increment()                { mCount++; }
        unsigned long decrement()       { return --mCount; }

    private:

        // Deleted
        
        Storage(const Storage&) = delete;
        Storage& operator=(const Storage&) = delete;
        
        // Member Variables
        
        std::string mName;
        FrameType mType;
        void *mData;
        size_t mSize;
        size_t mMaxSize;
        unsigned long mCount;
        
        SpinLock mLock;
        FrameLib_LocalAllocator *mAllocator;
    };
    
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
    
    FrameLib_LocalAllocator(const FrameLib_LocalAllocator&) = delete;
    FrameLib_LocalAllocator& operator=(const FrameLib_LocalAllocator&) = delete;
    
    // Find Storage by Name
    
    std::vector<Storage *>::iterator findStorage(const char *name);
    
    // Remove a Free Block after Allocation and Return the Pointer

    void *removeBlock(FreeBlock *block);
    
    // Member Variables
    
    FrameLib_GlobalAllocator *mAllocator;
    
    FreeBlock mFreeLists[numLocalFreeBlocks];
    FreeBlock *mTail;
    
    std::vector<Storage *> mStorage;
};

#endif
