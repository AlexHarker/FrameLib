
#include "FrameLib_ProcessingQueue.h"
#include "FrameLib_Global.h"
#include "FrameLib_DSP.h"

#include <algorithm>

// Worker Threads

void FrameLib_ProcessingQueue::WorkerThreads::doTask(unsigned int index)
{
    FrameLib_LocalAllocator *allocator = mQueue->mAllocators.get(index + 1);

    mQueue->serviceQueue(allocator);
    mQueue->mNumWorkersActive--;
}

// Constructor / Destructor

FrameLib_ProcessingQueue::FrameLib_ProcessingQueue(FrameLib_Global& global)
: mWorkers(this), mAllocators(global, FrameLib_Thread::maxThreads()), mNumItems(0), mNumWorkersActive(0), mMultithread(false), mTimedOut(false), mEntryObject(nullptr), mErrorReporter(global)
{
   mMultithread = global.getPriorities().mMultithread;
   mWorkers.start(global.getPriorities());
}

FrameLib_ProcessingQueue::~FrameLib_ProcessingQueue()
{
    mWorkers.join();
}

// Processing Queue

void FrameLib_ProcessingQueue::start(PrepQueue &queue)
{
    if (!queue.size() || mTimedOut)
        return;
    
    // Get the free blocks for this thread
    
    FrameLib_LocalAllocator *allocator = mAllocators.get(0);
    
    // Set the entry object and start the clock
    
    mEntryObject = queue.peek();
    mClock.start();
    
    // Enqueue items
    
    enqueue(queue);
    
    // Service queue until done
    
    while (true)
    {
        serviceQueue(allocator);
        
        if (mNumItems.load() == 0 || mTimedOut)
            break;
        
        // FIX - how long is a good time to yield for in a high performance thread?
        
        FrameLib_Thread::sleepCurrentThread(100);
    }
    
    // Clear the thread local allocator
    
    mAllocators.clear();
    
    // Check for time out
    
    if (mTimedOut)
    {
        mErrorReporter.reportError(kErrorDSP, mEntryObject->getProxy(), "FrameLib - DSP time out - FrameLib disabled in this context");
        
        // Clear the queue
        
        while (FrameLib_DSP *object = mQueue.pop())
            object->ThreadNode::mNext = nullptr;
        
        // Wait for all thhreads to return
        
        while (mNumWorkersActive.load());
        
        mNumItems = 0;
    }
}

void FrameLib_ProcessingQueue::add(PrepQueue &queue, FrameLib_DSP *addedBy)
{
    // Try to process this next in this thread, but if that isn't possible add to the queue
    
    if (!queue.size() || mTimedOut)
        return;
    
    // Try to process one item in this thread

    if (!addedBy->ThreadNode::mNext)
        addedBy->ThreadNode::mNext = queue.pop();
    
    // Add the rest to the queue
    
    if (queue.size())
        enqueue(queue);
}

void FrameLib_ProcessingQueue::enqueue(PrepQueue &queue)
{
    mNumItems += queue.size();
    mQueue.push(queue);
        
    // Wake workers
        
    wakeWorkers();
}

void FrameLib_ProcessingQueue::wakeWorkers()
{
    if (!mMultithread)
        return;
    
    int32_t numWorkersNeeded = 1;
    
    while (numWorkersNeeded > 0)
    {
        int32_t numItems = mNumItems;
        int32_t numWorkersActive = mNumWorkersActive.load();
        numWorkersNeeded = numItems - (numWorkersActive + 1);
        numWorkersNeeded = std::min(numWorkersNeeded, static_cast<int32_t>(mWorkers.size()) - numWorkersActive);
        
        if (numWorkersNeeded > 0)
        {
            if (compareAndSwap(mNumWorkersActive, numWorkersActive, numWorkersActive + numWorkersNeeded))
            {
                mWorkers.signal(numWorkersNeeded);
                numWorkersNeeded = 0;
            }
        }
    }
}

void FrameLib_ProcessingQueue::serviceQueue(FrameLib_LocalAllocator *allocator)
{
    unsigned long timedOutCount = 0;
    
    while (FrameLib_DSP *object = mQueue.pop())
    {
        while (object && !mTimedOut)
        {
            object->dependenciesReady(allocator);
            FrameLib_DSP *newObject = object->ThreadNode::mNext;
            object->ThreadNode::mNext = nullptr;
            object = newObject;
            
            // Check for time out
            
            if (++timedOutCount == sProcessPerTimeCheck)
            {
                if (checkForTimeOut())
                    return;
                timedOutCount = 0;
            }
        }
        mNumItems--;
    }
    
    checkForTimeOut();
}

bool FrameLib_ProcessingQueue::checkForTimeOut()
{
    if (!mTimedOut && mClock.elapsed() > sMaxTime)
        mTimedOut = true;
    
    return isTimedOut();
}

// Audio Queue

FrameLib_AudioQueue::~FrameLib_AudioQueue()
{
    if (mUser)
        mUser->mProcessingQueue->start(*this);
}
