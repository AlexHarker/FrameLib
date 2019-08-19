
#include "FrameLib_ProcessingQueue.h"
#include "FrameLib_Global.h"
#include "FrameLib_DSP.h"

#include <algorithm>

// N.B. - the current implementation of a lock-free queue is taken from:
// Michael, Maged M., and Michael L. Scott.
// Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms.
// No. TR-600. ROCHESTER UNIV NY DEPT OF COMPUTER SCIENCE, 1995.

template <class T>
bool compareAndSwapS(std::atomic<T>& value, T comparand, T exchange)
{
    return value.compare_exchange_strong(comparand, exchange, std::memory_order_relaxed);
}

// Constructor / Destructor

FrameLib_ProcessingQueue::FrameLib_ProcessingQueue(FrameLib_Global& global)
: mWorkers(this), mDummyNode(nullptr), mNumItems(0), mNumWorkersActive(0), mTimedOut(false), mErrorReporter(global)
{
    init();
    
    for (unsigned int i = 0; i < FrameLib_Thread::maxThreads(); i++)
        mFreeBlocks.add(new FrameLib_FreeBlocks(global));
    
    mWorkers.start();
}

FrameLib_ProcessingQueue::~FrameLib_ProcessingQueue()
{
    mWorkers.join();
}

// Processing Queue

void FrameLib_ProcessingQueue::add(FrameLib_DSP *object, FrameLib_DSP *addedBy)
{
    assert(object->mInputTime != FrameLib_TimeFormat::largest() && "Object has already reached the end of time");
    assert((!object->mNode.mNextInThread) && "Object is already in the queue");
    
    // Try to process this next in this thread, but if that isn't possible add to the queue
    
    if (!addedBy || addedBy->mNode.mNextInThread)
    {
        int32_t numItems = ++mNumItems;
        int32_t numWorkersActive = mNumWorkersActive.load();// + (addedBy ? 0 : 1);
        int32_t numWorkersNeeded = numItems - numWorkersActive;
    
        numWorkersNeeded = std::min(numWorkersNeeded, static_cast<int32_t>(mWorkers.size()) - numWorkersActive);
    
        if (numWorkersNeeded > 0)
            mWorkers.signal(numWorkersNeeded);
    
        enqueue(object);
        
        if (!addedBy)
            serviceQueue(0);
    }
    else
        addedBy->mNode.mNextInThread = object;
}

void FrameLib_ProcessingQueue::serviceQueue(int32_t index)
{
    FrameLib_FreeBlocks *blocks = mFreeBlocks[index].get();
    mNumWorkersActive++;
    
    while (true)
    {
        while (FrameLib_DSP *object = dequeue())
        {
            while (object)
            {
                object->dependenciesReady(blocks);
                FrameLib_DSP *newObject = object->mNode.mNextInThread;
                object->mNode.mNextInThread = nullptr;
                object = newObject;
            }
            mNumItems--;
        }
        
        // FIX - quick reliable and non-contentious exit strategies are needed here...
        
        if (mNumItems.load() == 0)
        {
            if (index == 0)
            {
                for (auto it = mFreeBlocks.begin(); it != mFreeBlocks.end(); it++)
                    it->get()->clear();
            }
            mNumWorkersActive--;
            return;
        }
        
        // FIX - how long is a good time to yield for in a high performance thread?
        
        // Reduce contention down and give way to other threads
        
        FrameLib_Thread::sleepCurrentThread(1000);
    }
}

void FrameLib_ProcessingQueue::init()
{
    mHead = NodePointer(&mDummyNode, 0);
    mTail = NodePointer(&mDummyNode, 0);
}

void FrameLib_ProcessingQueue::enqueue(FrameLib_DSP *object)
{
    Node *node = &object->mNode;
    node->mNext = NodePointer();
    
    while (true)
    {
        NodePointer tail = mTail;
        NodePointer next = tail.mPointer->mNext;
        
        // Check that tail has not changed
        
        if (tail == mTail)
        {
            if (next.mPointer == nullptr)
            {
                // If tail was pointing to the last node then we can attempt to enqueue
                
                if (compareAndSwapS(tail.mPointer->mNext, next, NodePointer{node, next.mCount + 1}))
                {
                    // Try to update tail to the inserted node
                    
                    compareAndSwapS(mTail, tail, NodePointer{node, tail.mCount + 1});
                    return;
                }
            }
            else
            {
                // Try to advance tail if falling behind

                compareAndSwapS(mTail, tail, NodePointer{next.mPointer, tail.mCount + 1});
            }
        }
    }
}

FrameLib_DSP *FrameLib_ProcessingQueue::dequeue()
{
    while (true)
    {
        // Read head, tail and next
        
        NodePointer head = mHead;
        NodePointer tail = mTail;
        NodePointer next = head.mPointer->mNext;
        
        // Check that head has not changed
        
        if (head == mHead)
        {
            // Check if either the queue is empty or tail is falling behind
            
            if (head.mPointer == tail.mPointer)
            {
                // Check for empty queue
                
                if (next.mPointer == nullptr)
                    return nullptr;
                
                // Try to advance tail if falling behind
                
                compareAndSwapS(mTail, tail, NodePointer(next.mPointer, tail.mCount + 1));
            }
            else
            {
                // Read the pointer before CAS otherwise another dequeue may free the next node
                
                FrameLib_DSP *object = next.mPointer->mOwner;
                
                // Attempt to change the head and complete dequeue
                
                if (compareAndSwapS(mHead, head, NodePointer(next.mPointer, head.mCount + 1)))
                {
                    head.mPointer->mNext = NodePointer();
                    return object;
                }
            }
        }
    }
    
    return nullptr;
}


// FIX - old version - need to add timeout to the above
/*
void FrameLib_ProcessingQueue::add(FrameLib_DSP *object)
{
    assert(object->mInputTime != FrameLib_TimeFormat::largest() && "Object has already reached the end of time");
    assert((!object->mNext || mTop == object) && "Object is already in the queue and not at the top");

    if (mTimedOut)
        return;
    
    if (!mTop)
    {
        // Queue is empty - add and start processing the queue
        
        mTop = mTail = object;
        
        // Get time
        
        mClock.start();
        unsigned long count = 0;
        
        while (mTop)
        {
            object = mTop;
            object->dependenciesReady();
            mTop = object->mNext;
            object->mNext = nullptr;
            
            // Every so often check whether we're taking too long
            
            if (++count == sProcessPerTimeCheck)
            {
                if (mClock.elapsed() > sMaxTime)
                {
                    mTimedOut = true;
                    
                    // Clear the list
                    
                    while (mTop)
                    {
                        mErrorReporter.reportError(kErrorDSP, mTop->getProxy(), "FrameLib - DSP time out - FrameLib is disabled in this context until this is resolved");
                        object = mTop;
                        mTop = object->mNext;
                        object->mNext = nullptr;
                    }
                }
                count = 0;
            }
        }
        
        mTail = nullptr;
    }
    else
    {
        // Add to the queue (which is already processing)
        
        mTail->mNext = object;
        mTail = object;
    }
}*/
