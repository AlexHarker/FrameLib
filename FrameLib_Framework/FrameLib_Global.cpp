
#include "FrameLib_Global.h"

FrameLib_Global::FrameLib_Global() : mCount(1) {}

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

FrameLib_LocalAllocator *FrameLib_Global::getAllocator(void *ref)
{
    FrameLib_LocalAllocator *allocator = mLocalAllocators.find(ref);
    
    if (!allocator)
    {
        allocator = new FrameLib_LocalAllocator(&mAllocator);
        mLocalAllocators.add(allocator, ref);
    }
    
    return allocator;
}

FrameLib_MultiChannel::ConnectionQueue *FrameLib_Global::getConnectionQueue(void *ref)
{
    FrameLib_MultiChannel::ConnectionQueue *queue = mConnectionQueues.find(ref);
    
    if (!queue)
    {
        queue = new FrameLib_MultiChannel::ConnectionQueue();
        mConnectionQueues.add(queue, ref);
    }
    
    return queue;
}

FrameLib_DSP::DSPQueue *FrameLib_Global::getDSPQueue(void *ref)
{
    FrameLib_DSP::DSPQueue *queue = mDSPQueues.find(ref);
    
    if (!queue)
    {
        queue = new FrameLib_DSP::DSPQueue();
        mDSPQueues.add(queue, ref);
    }
    
    return queue;
}

void FrameLib_Global::releaseAllocator(void *ref) { mLocalAllocators.release(ref); }

void FrameLib_Global::releaseConnectionQueue(void *ref) { mConnectionQueues.release(ref); }

void FrameLib_Global::releaseDSPQueue(void *ref) { mDSPQueues.release(ref); }
