
#ifndef FRAMELIB_GLOBAL_H
#define FRAMELIB_GLOBAL_H

#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"
#include "FrameLib_Memory.h"
#include "FrameLib_ProcessingQueue.h"
#include "FrameLib_Threading.h"

#include <vector>

// The Global Object

class FrameLib_Global : public FrameLib_ErrorReporter
{

    friend class FrameLib_Context;
    
private:

    template <class T> class PointerSet
    {
        // A simple countable pointer with a reference address
        
        struct CountablePointer
        {
            CountablePointer(T* object, void *reference) : mObject(object), mReference(reference), mCount(1) {}
            
            T *mObject;
            void *mReference;
            long mCount;
        };
        
    public:
        
        // Add, find or release an object by reference address
        
        void add(T *object, void *reference);
        T *find(void *reference);
        void release(void *reference);
        
    private:
        
        // Type definition for concision and internal pointers
        
        typedef std::vector<CountablePointer> PointerVector;
        typedef typename PointerVector::iterator Iterator;
        
        PointerVector mPointers;
    };
    
    // Constructor / Destructor
    
    FrameLib_Global(FrameLib_ErrorReporter::HostNotifier *notifier) : FrameLib_ErrorReporter(notifier), mAllocator(*this), mCount(0) {}
    ~FrameLib_Global() {};
    
    // Deleted
    
    FrameLib_Global(const FrameLib_Global&) = delete;
    FrameLib_Global& operator=(const FrameLib_Global&) = delete;
    
public:
    
    // Retrieve and release the global object
    
    static FrameLib_Global *get(FrameLib_Global **global, FrameLib_ErrorReporter::HostNotifier *notifier = NULL);
    static void release(FrameLib_Global **global);
        
private:
    
    // Methods to retrieve common objects

    FrameLib_LocalAllocator *getAllocator(void *reference);
    FrameLib_ProcessingQueue *getProcessingQueue(void *reference);
    
    // Methods to release common objects

    void releaseAllocator(void *reference);
    void releaseProcessingQueue(void *reference);
    
    // Reference Counting / Auto-deletion
    
    void increment();
    FrameLib_Global *decrement();
    
    // Common Objects
    
    FrameLib_GlobalAllocator mAllocator;
    
    PointerSet<FrameLib_LocalAllocator> mLocalAllocators;
    PointerSet<FrameLib_ProcessingQueue> mProcessingQueues;
    
    SpinLock mLock;
    long mCount;
};

#endif
