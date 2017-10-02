
#include "FrameLib_Global.h"

// Retrieve and release the global object

FrameLib_Global *FrameLib_Global::get(FrameLib_Global **global)
{
    if (!*global)
        *global = new FrameLib_Global();

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
        queue = new FrameLib_ProcessingQueue();
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
