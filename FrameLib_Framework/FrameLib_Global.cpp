
#include "FrameLib_Global.h"

// Retrieve and release the global object

FrameLib_Global *FrameLib_Global::get(FrameLib_Global **global)
{
    if (*global)
    {
        (*global)->increment();
        return *global;
    }
    
    *global = new FrameLib_Global();
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

FrameLib_MultiChannel::ConnectionQueue *FrameLib_Global::getConnectionQueue(void *reference)
{
    FrameLib_MultiChannel::ConnectionQueue *queue = mConnectionQueues.find(reference);
    
    if (!queue)
    {
        queue = new FrameLib_MultiChannel::ConnectionQueue();
        mConnectionQueues.add(queue, reference);
    }
    
    return queue;
}

FrameLib_DSPQueue *FrameLib_Global::getDSPQueue(void *reference)
{
    FrameLib_DSPQueue *queue = mDSPQueues.find(reference);
    
    if (!queue)
    {
        queue = new FrameLib_DSPQueue();
        mDSPQueues.add(queue, reference);
    }
    
    return queue;
}

// Methods to release common objects

void FrameLib_Global::releaseAllocator(void *reference)
{
    mLocalAllocators.release(reference);
}

void FrameLib_Global::releaseConnectionQueue(void *reference)
{
    mConnectionQueues.release(reference);
}

void FrameLib_Global::releaseDSPQueue(void *reference)
{
    mDSPQueues.release(reference);
}

// Reference Counting / Auto-deletion

void FrameLib_Global::increment()
{
    ++mCount;
}

FrameLib_Global *FrameLib_Global::decrement()
{
    if (--mCount < 1)
    {
        delete this;
        return NULL;
    }
    
    return this;
}
