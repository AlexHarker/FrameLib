
#ifndef FRAMELIB_GLOBAL_H
#define FRAMELIB_GLOBAL_H

#include "FrameLib_Memory.h"
#include "FrameLib_DSP.h"
#include "FrameLib_MultiChannel.h"
#include <vector>

// A template class for storing reference counted pointers against reference addresses (representing contexts)

template<class T> struct FrameLib_PointerSet
{
    // A simple countable pointer with a reference address
    
    template<class U> struct CountablePointer
    {
        CountablePointer(T* object, void *reference) : mObject(object), mReference(reference), mCount(1) {}
        
        T *mObject;
        void *mReference;
        long mCount;
    };
    
    // Type definition for concision
    
    typedef std::vector<CountablePointer<T> > VectorType;
    
    // Find a pre-exisitng object by reference address
    
    T *find(void *reference)
    {
        for (typename VectorType::iterator it = mPointers.begin(); it != mPointers.end(); it++)
        {
            if (it->mReference == reference)
            {
                it->mCount++;
                return it->mObject;
            }
        }
        
        return NULL;
    }
    
    // Release a pre-existing object by reference address
    
    void release(void *reference)
    {
        for (typename VectorType::iterator it = mPointers.begin(); it != mPointers.end(); it++)
        {
            if (it->mReference == reference)
            {
                if (--it->mCount < 1)
                {
                    delete it->mObject;
                    mPointers.erase(it);
                }
                
                return;
            }
        }
    }
    
    // Add a new object given a pointer (transferring ownership) and a reference address
    
    void add(T *object, void *reference)
    {
        mPointers.push_back(CountablePointer<T>(object, reference));
    }
    
private:
    
    // Internal set of pointers
    
    std::vector<CountablePointer<T> > mPointers;
};


// The global object

class FrameLib_Global
{
    FrameLib_Global();
    ~FrameLib_Global(){};
    FrameLib_Global(const FrameLib_Global&);
    FrameLib_Global& operator=(const FrameLib_Global&);
    
    // Reference Counting / Auto-deletion
    
    void increment();
    FrameLib_Global *decrement();
    
public:
    
    // Retrieve and release the global object
    
    static FrameLib_Global *get(FrameLib_Global **global);
    static void release(FrameLib_Global **global);
    
    // Methods to retrieve common objects

    FrameLib_LocalAllocator *getAllocator(void *reference);
    FrameLib_MultiChannel::ConnectionQueue *getConnectionQueue(void *reference);
    FrameLib_DSP::DSPQueue *getDSPQueue(void *reference);
    
    // Methods to release common objects

    void releaseAllocator(void *reference);
    void releaseConnectionQueue(void *reference);
    void releaseDSPQueue(void *reference);
    
private:
    
    // Common Objects
    
    FrameLib_GlobalAllocator mAllocator;
    
    FrameLib_PointerSet<FrameLib_LocalAllocator> mLocalAllocators;
    FrameLib_PointerSet<FrameLib_MultiChannel::ConnectionQueue> mConnectionQueues;
    FrameLib_PointerSet<FrameLib_DSP::DSPQueue> mDSPQueues;
    
    long mCount;
};

#endif
