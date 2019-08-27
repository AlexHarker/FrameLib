
#include "FrameLib_ProcessingQueue.h"
#include "FrameLib_Global.h"
#include "FrameLib_DSP.h"

#include <algorithm>

// Constructor / Destructor

FrameLib_ProcessingQueue::FrameLib_ProcessingQueue(FrameLib_Global& global)
: mWorkers(this), mNumItems(0), mNumWorkersActive(0), mMultithread(true), mTimedOut(false), mEntryObject(nullptr), mErrorReporter(global)
{
    for (unsigned int i = 0; i < FrameLib_Thread::maxThreads(); i++)
        mFreeBlocks.add(new FrameLib_FreeBlocks(global));
    
    mWorkers.start();
}

FrameLib_ProcessingQueue::~FrameLib_ProcessingQueue()
{
    mWorkers.join();
}

// Processing Queue

void FrameLib_ProcessingQueue::start(PrepQueue &queue)
{
    if (queue.size() && !mTimedOut)
    {
        mEntryObject = queue.peek();
        mClock.start();
        mNumItems += queue.size();
        wakeWorkers(false); // !addedBy
        mQueue.enqueue(queue);
        serviceQueue(0);
    }
}

void FrameLib_ProcessingQueue::start(FrameLib_DSP *object)
{
    assert(object->mInputTime != FrameLib_TimeFormat::largest() && "Object has already reached the end of time");
    assert((!object->ThreadNode::mNext) && "Object is already in the queue");
    
    if (mTimedOut)
        return;
    
    mEntryObject = object;
    mClock.start();
    mNumItems++;
    wakeWorkers(false); // !addedBy
    mQueue.enqueue(object);
    serviceQueue(0);
}

void FrameLib_ProcessingQueue::add(PrepQueue &queue, FrameLib_DSP *addedBy)
{
    // Try to process this next in this thread, but if that isn't possible add to the queue
    
    if (!queue.size() || mTimedOut)
        return;
    
    // Try to process one item in this thread

    if (!addedBy->ThreadNode::mNext)
        addedBy->ThreadNode::mNext = queue.pop();
    
    if (queue.size())
    {
        mNumItems += queue.size();
        wakeWorkers(false); // !addedBy
        mQueue.enqueue(queue);
    }
}

void FrameLib_ProcessingQueue::add(FrameLib_DSP *object, FrameLib_DSP *addedBy)
{
    assert(object->mInputTime != FrameLib_TimeFormat::largest() && "Object has already reached the end of time");
    assert((!object->ThreadNode::mNext) && "Object is already in the queue");
    
    if (mTimedOut)
        return;
    
    // Try to process this next in this thread, but if that isn't possible add to the queue

    if (!addedBy->ThreadNode::mNext)
    {
        addedBy->ThreadNode::mNext = object;
    }
    else
    {
        mNumItems++;
        wakeWorkers(false); // !addedBy
        mQueue.enqueue(object);
    }
}

void FrameLib_ProcessingQueue::wakeWorkers(bool countThisThread)
{
    int32_t numItems = mNumItems;
    
    if (!mMultithread)
        return;
    
    int32_t numWorkersActive = mNumWorkersActive.load() + (countThisThread ? 1 : 0);
    int32_t numWorkersNeeded = numItems - numWorkersActive;
    
    numWorkersNeeded = std::min(numWorkersNeeded, static_cast<int32_t>(mWorkers.size()) - numWorkersActive);
    
    if (numWorkersNeeded > 0)
        mWorkers.signal(numWorkersNeeded);
}

void FrameLib_ProcessingQueue::serviceQueue(int32_t index)
{
    FrameLib_FreeBlocks *blocks = mFreeBlocks[index].get();
    mNumWorkersActive++;
    
    unsigned long timedOutCount = 0;
    
    while (true)
    {
        while (FrameLib_DSP *object = mQueue.dequeue())
        {
            while (object && !mTimedOut)
            {
                object->dependenciesReady(blocks);
                FrameLib_DSP *newObject = object->ThreadNode::mNext;
                object->ThreadNode::mNext = nullptr;
                object = newObject;
                
                // Check for time out
                
                if (++timedOutCount == sProcessPerTimeCheck)
                {
                    if (mClock.elapsed() > sMaxTime)
                        mTimedOut = true;
                    timedOutCount = 0;
                }
            }
            mNumItems--;
            
            if (mTimedOut)
                break;
        }
        
        // FIX - quick reliable and non-contentious exit strategies are needed here...
        
        if (mNumItems.load() == 0 || mTimedOut)
        {
            if (index == 0)
                cleanup();
            mNumWorkersActive--;
            return;
        }
        
        // FIX - how long is a good time to yield for in a high performance thread?
        // Reduce contention down and give way to other threads
        
        FrameLib_Thread::sleepCurrentThread(100);
    }
}

void FrameLib_ProcessingQueue::cleanup()
{
    for (auto it = mFreeBlocks.begin(); it != mFreeBlocks.end(); it++)
        it->get()->clear();
    
    if (mTimedOut)
    {
        mErrorReporter.reportError(kErrorDSP, mEntryObject->getProxy(), "FrameLib - DSP time out - FrameLib is disabled in this context until this is resolved");
        
        // Clear the queue
        
        while (FrameLib_DSP *object = mQueue.dequeue())
            object->ThreadNode::mNext = nullptr;
        
        mNumItems = 0;
    }
}

// Audio Queue

FrameLib_AudioQueue::~FrameLib_AudioQueue()
{
    if (mUser)
        mUser->mProcessingQueue->start(*this);
}

