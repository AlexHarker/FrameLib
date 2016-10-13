
#ifndef FRAMELIB_GLOBAL_H
#define FRAMELIB_GLOBAL_H

#include "FrameLib_Memory.h"
#include "FrameLib_DSP.h"
#include "FrameLib_MultiChannel.h"
#include <vector>

template<class T> struct FrameLib_CountedPointer
{
    FrameLib_CountedPointer(T* obj, void *ref) : mObj(obj), mRef(ref), mCount(1) {}
    
    T *getObj()     { return mObj; }
    void *getRef()  { return mRef; }
    
    void increment() { ++mCount; }
    bool decrement()
    {
        if (--mCount < 1)
        {
            delete mObj;
            mObj = NULL;
            return true;
        }
        
        return false;
    }
    
private:
    
    T *mObj;
    void *mRef;
    long mCount;
};


template<class T> struct FrameLib_PointerSet : private std::vector<FrameLib_CountedPointer<T> >
{
    typedef std::vector<FrameLib_CountedPointer<T> > VectorType;
    
    T *find(void *ref)
    {
        for (typename VectorType::iterator it = VectorType::begin(); it != VectorType::end(); it++)
        {
            if (it->getRef() == ref)
            {
                it->increment();
                return it->getObj();
            }
        }
        
        return NULL;
    }
    
    void release(void *ref)
    {
        for (typename VectorType::iterator it = VectorType::begin(); it != VectorType::end(); it++)
        {
            if (it->getRef() == ref)
            {
                if (it->decrement())
                    VectorType::erase(it);
                
                return;
            }
        }
    }
    
    void add(T *obj, void *ref)
    {
        VectorType::push_back(FrameLib_CountedPointer<T>(obj, ref));
    }
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

    FrameLib_LocalAllocator *getAllocator(void *ref);
    FrameLib_MultiChannel::ConnectionQueue *getConnectionQueue(void *ref);
    FrameLib_DSP::DSPQueue *getDSPQueue(void *ref);
    
    // Methods to release common objects

    void releaseAllocator(void *ref);
    void releaseConnectionQueue(void *ref);
    void releaseDSPQueue(void *ref);
    
private:
    
    // Common Objects
    
    FrameLib_GlobalAllocator mAllocator;
    
    FrameLib_PointerSet<FrameLib_LocalAllocator> mLocalAllocators;
    FrameLib_PointerSet<FrameLib_MultiChannel::ConnectionQueue> mConnectionQueues;
    FrameLib_PointerSet<FrameLib_DSP::DSPQueue> mDSPQueues;
    
    long mCount;
};

#endif
