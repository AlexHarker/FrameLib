
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
: mWorkers(this), mAllocators(global, FrameLib_Thread::maxThreads()), mNumItems(0), mNumWorkersActive(0), mMultithread(false), mMaxTime(0), mRelativeTimeOut(4.0), mAbsoluteTimeOut(10.0), mTimedOut(false), mEntryObject(nullptr), mErrorReporter(global)
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
    calculateTimeOutMax();
    mClock.start();
    
    // Enqueue items
    
    enqueue(queue);
    
    // Service queue until done
    
    while (true)
    {
        serviceQueue(allocator);
        
        if (mNumItems.load() == 0 || mTimedOut)
            break;
                
        FrameLib_Thread::sleepCurrentThread(100);
    }
    
    // Clear the thread local allocator
    
    mAllocators.clear();
    
    // Check for time out
    
    if (mTimedOut)
    {
        mErrorReporter(kErrorDSP, mEntryObject->getProxy(), "FrameLib - DSP time out - FrameLib disabled in this context");
        
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
    DenormalHandling denormHandler;
    
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

void FrameLib_ProcessingQueue::setTimeOuts(double relative, double absolute)
{
    // N.B. Relative time as a ratio, absolute time in seconds
    
    mRelativeTimeOut = std::max(relative, 0.0);
    mAbsoluteTimeOut = std::max(absolute, 0.0);
}

bool FrameLib_ProcessingQueue::checkForTimeOut()
{
    if (mMaxTime && !mTimedOut && mClock.elapsed() > mMaxTime)
        mTimedOut = true;
    
    return isTimedOut();
}

void FrameLib_ProcessingQueue::calculateTimeOutMax()
{
    auto timeConvert = [](double x) { return x ? std::max(x * 1000000.0, 1.0) : 0.0; };
    
    const double samplingRate = mEntryObject->mSamplingRate;
    double vectorTime = (getBlockEndTime() - getBlockStartTime()) / samplingRate;
    
    const double maxRelativeTime = timeConvert(vectorTime * mRelativeTimeOut);
    const double maxAbsoluteTime = timeConvert(mAbsoluteTimeOut);

    mMaxTime = static_cast<long long>(round(std::max(maxRelativeTime, maxAbsoluteTime)));
}

FrameLib_TimeFormat FrameLib_ProcessingQueue::getBlockStartTime() const
{
    return mEntryObject ? mEntryObject->getBlockStartTime() : FrameLib_TimeFormat();
}

FrameLib_TimeFormat FrameLib_ProcessingQueue::getBlockEndTime() const
{
    return mEntryObject ? mEntryObject->getBlockEndTime() : FrameLib_TimeFormat();
}

// Audio Queue

FrameLib_AudioQueue::~FrameLib_AudioQueue()
{
    if (mUser)
        mUser->mProcessingQueue->start(*this);
}
