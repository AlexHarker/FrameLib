
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

/**

\defgroup Memory

*/

/**
 
 \class FrameLib_GlobalAllocator
 
 \ingroup Memory
 
 \brief a global threadsafe memory allocator suitable for realtime usage.
 
 */

class FrameLib_GlobalAllocator
{

private:
    
    /**
     
     \class CoreAllocator
     
     \brief the underlying single-threaded allocator.
     
     */
    
    class CoreAllocator
    {        
        /**
         
         \struct Pool
         
         \brief a memory pool from system memory.
         
         */
        
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
        
        /**
         
         \class NewThread
         
         \brief a thread for allocating new memory pools from system memory.
         
         */
        
        class NewThread final : public FrameLib_DelegateThread
        {
            
        public:
            
            NewThread(CoreAllocator *allocator) : FrameLib_DelegateThread(FrameLib_Thread::kHighPriority), mAllocator(allocator) {}
            
        private:
            
            void doTask() override { mAllocator->addScheduledPool(); };
            
            CoreAllocator *mAllocator;
        };
        
        /**
         
         \class FreeThread
         
         \brief a thread for freeing memory pools back to system memory.
         
         */

        class FreeThread final : public FrameLib_TriggerableThread
        {
            
        public:
            
            FreeThread(CoreAllocator *allocator) : FrameLib_TriggerableThread(FrameLib_Thread::kLowPriority), mAllocator(allocator) {}
            
        private:
            
            void doTask() override { mAllocator->destroyScheduledPool(); };
            
            CoreAllocator *mAllocator;
        };
                
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
    
public:

    /**
     
     \class Pruner
     
     \brief an RAII utility for repeated deallocation with only a single lock.
     
     */
        
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
        
        // Non-copyable
        
        Pruner(const Pruner&) = delete;
        Pruner& operator=(const Pruner&) = delete;
        
        void dealloc(void *ptr) { mAllocator->mAllocator.dealloc(ptr); }

    private:
        
        // Allocator
        
        FrameLib_GlobalAllocator *mAllocator;
    };

    // ************************************************************************************** //

    // Constructor / Destructor
    
    FrameLib_GlobalAllocator(FrameLib_ErrorReporter& errorReporter) : mAllocator(errorReporter) {}
    ~FrameLib_GlobalAllocator() {}
    
    // Non-copyable
    
    FrameLib_GlobalAllocator(const FrameLib_GlobalAllocator&) = delete;
    FrameLib_GlobalAllocator& operator=(const FrameLib_GlobalAllocator&) = delete;
    
    // Allocate / Deallocate Memory
    
    void *alloc(size_t size);
    void dealloc(void *ptr);
    
    // Alignment Helpers
    
    static size_t getAlignment();
    static size_t alignSize(size_t x);
    
private:
   
    // Member Variables
    
    FrameLib_SpinLock mLock;
    CoreAllocator mAllocator;
};


/**
 
 \class FrameLib_LocalAllocator
 
 \ingroup Memory

 \brief a memory allocator suitable for usage in a given FrameLib context.
 
 \sa FrameLib_Context
 
 */

class FrameLib_LocalAllocator
{
    static const int numLocalFreeBlocks = 16;

    /**
     
     \struct FreeBlock
     
     \brief a memory block that can be addressed as part of double-linked list.
     
     */
    
    struct FreeBlock
    {
        FreeBlock() : mMemory(nullptr), mSize(0), mPrev(nullptr), mNext(nullptr) {}
        
        void *mMemory;
        size_t mSize;
        
        FreeBlock *mPrev;
        FreeBlock *mNext;
    };
    
public:

    /**
     
     \class Storage
     
     \brief named storage local to a specific context.
     
     */
    
    class Storage
    {
        using Serial = FrameLib_Parameters::Serial;
        
        friend class FrameLib_LocalAllocator;

    public:
        
        /**
         
         \class Access
         
         \brief an RAII utility for safely accessing a Storage object.
         
         */
        
        class Access
        {
            
        public:
            
            // Constructor and Destructor
            
            Access(Storage *storage) : mStorage(storage)    { mStorage->mLock.acquire(); }
            ~Access()                                       { mStorage->mLock.release(); }
            
            // Non-copyable
            
            Access(const Access&) = delete;
            Access& operator=(const Access&) = delete;
            
            // Getters
            
            FrameType getType() const               { return mStorage->getType(); }
            double *getVector() const               { return mStorage->getVector(); }
            unsigned long getVectorSize() const     { return mStorage->getVectorSize(); }
            unsigned long getTaggedSize() const     { return mStorage->getTaggedSize(); }
            Serial *getTagged() const               { return mStorage->getTagged(); }
            
            // Resize
            
            void resize(bool tagged, size_t size)   { mStorage->resize(tagged, size); }
            
        private:
            
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
        
        // Non-copyable
        
        Storage(const Storage&) = delete;
        Storage& operator=(const Storage&) = delete;
        
        // Reference Counting
        
        void increment()                { mCount++; }
        unsigned long decrement()       { return --mCount; }

    private:

        // Member Variables
        
        std::string mName;
        FrameType mType;
        void *mData;
        size_t mSize;
        size_t mMaxSize;
        unsigned long mCount;
        
        FrameLib_SpinLock mLock;
        FrameLib_LocalAllocator *mAllocator;
    };
    
    // Constructor / Destructor
    
    FrameLib_LocalAllocator(FrameLib_GlobalAllocator *allocator);
    ~FrameLib_LocalAllocator();
    
    // Non-copyable
    
    FrameLib_LocalAllocator(const FrameLib_LocalAllocator&) = delete;
    FrameLib_LocalAllocator& operator=(const FrameLib_LocalAllocator&) = delete;
    
    
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
