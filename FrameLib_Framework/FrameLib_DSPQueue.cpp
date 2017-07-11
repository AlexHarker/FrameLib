
#include "FrameLib_DSPQueue.h"
#include "FrameLib_DSP.h"

void FrameLib_DSPQueue::process(FrameLib_DSP *object)
{
    while (object)
    {
        object->dependenciesReady();
        FrameLib_DSP *newObject = object->mNextInThread;
        object->mNextInThread = NULL;
        object = newObject;
    }
    mQueueSize--;
}

void FrameLib_DSPQueue::start(FrameLib_DSP *object)
{
    mInQueue++;
    mQueueSize++;
    mWorkers.signal(1);
    object->mNextInThread = NULL;
    process(object);
    serviceQueue();
    --mInQueue;
}

void FrameLib_DSPQueue::add(FrameLib_DSP *object)
{
    int32_t queueSize = ++mQueueSize;
    int32_t inQueue = mInQueue;
    int32_t sigSize = queueSize - inQueue;
    int32_t maxSigSize = mWorkers.size() - inQueue;
                          
    sigSize = sigSize > maxSigSize ? maxSigSize : sigSize;
    
    if (sigSize > 0)
        mWorkers.signal(sigSize);
    
    object->mNextInThread = NULL;
    OSAtomicFifoEnqueue(&mQueue, &object->mQueueItem, offsetof(QueueItem, mNext));
}

void FrameLib_DSPQueue::serviceQueue()
{
    struct timespec a;
    a.tv_sec = 0;
    a.tv_nsec = 100;
    
    while (true)
    {
        while (QueueItem *next = (QueueItem *) OSAtomicFifoDequeue(&mQueue, offsetof(QueueItem, mNext)))
            process(next->mThis);
    
        // FIX - quick reliable and non-contentious exit strategies are needed here...
        
        if (mQueueSize == 0)
            return;
        
        // FIX - how long is a good time to yield for in a high performance thread?
        
        // Keep pointless contention down and give way to other threads?

        nanosleep(&a,NULL);
    }
}
