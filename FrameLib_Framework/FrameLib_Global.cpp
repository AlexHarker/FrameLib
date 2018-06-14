
#include "FrameLib_Global.h"

// The PointerSet Template

// Add an object given a pointer (transferring ownership) and a reference address

template <class T>
void FrameLib_Global::PointerSet<T>::add(T *object, void *reference)
{
    mPointers.push_back(CountablePointer(object, reference));
}

// Find a pre-exising object by reference address

template <class T>
T *FrameLib_Global::PointerSet<T>::find(void *reference)
{
    for (auto it = mPointers.begin(); it != mPointers.end(); it++)
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

template <class T>
void FrameLib_Global::template PointerSet<T>::release(void *reference)
{
    for (auto it = mPointers.begin(); it != mPointers.end(); it++)
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

// The Main Global Class

// Retrieve and release the global object

FrameLib_Global *FrameLib_Global::get(FrameLib_Global **global, FrameLib_ErrorReporter::HostNotifier *notifier)
{
    if (!*global)
        *global = new FrameLib_Global(notifier);

    (*global)->increment();
    return *global;
}

void FrameLib_Global::release(FrameLib_Global **global)
{
    *global = (*global)->decrement();
}

// Methods to retrieve common objects

FrameLib_LocalAllocator *FrameLib_Global::getAllocator(void *reference)
{
    FrameLib_LocalAllocator *allocator = mLocalAllocators.find(reference);
    
    if (!allocator)
    {
        allocator = new FrameLib_LocalAllocator(&mAllocator);
        mLocalAllocators.add(allocator, reference);
    }
    
    return allocator;
}

FrameLib_ProcessingQueue *FrameLib_Global::getProcessingQueue(void *reference)
{
    FrameLib_ProcessingQueue *queue = mProcessingQueues.find(reference);
    
    if (!queue)
    {
        queue = new FrameLib_ProcessingQueue(*this);
        mProcessingQueues.add(queue, reference);
    }
    
    return queue;
}

// Methods to release common objects

void FrameLib_Global::releaseAllocator(void *reference)
{
    mLocalAllocators.release(reference);
}

void FrameLib_Global::releaseProcessingQueue(void *reference)
{
    mProcessingQueues.release(reference);
}

// Reference Counting / Auto-deletion

void FrameLib_Global::increment()
{
    SpinLockHolder lock(&mLock);
    ++mCount;
}

FrameLib_Global *FrameLib_Global::decrement()
{
    SpinLockHolder lock(&mLock);
    
    if (--mCount < 1)
    {
        // N.B. - The spinlock will attempt to lock itself in its deconstructor, so we can't hold the lock
        
        lock.destroy();
        delete this;
        return NULL;
    }
    
    return this;
}
