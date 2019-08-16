
#ifndef FRAMELIB_PROCESSINGQUEUE_H
#define FRAMELIB_PROCESSINGQUEUE_H

#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"
#include "FrameLib_Threading.h"

#include <chrono>

// FIX - mac only
#include <libkern/OSAtomicQueue.h>

// Forward Declarations

class FrameLib_DSP;

/**
 
 @defgroup ProcessingQueue Processing Queue
 
 */

/**
 
 @class FrameLib_ProcessingQueue
 
 @ingroup ProcessingQueue
 
 @brief a minimal processing queue that is used to non-recursively process FrameLIB_DSP objects in a network.
 
 */

class FrameLib_ProcessingQueue
{
    /**
     
     @class IntervalSecondsClock
     
     @brief a clock for measuring time intervals in seconds.
     
     */

    class IntervalSecondsClock
    {
        
    public:
        
        void start() { mStartTime = getTime(); }
        long long elapsed() { return std::chrono::duration_cast<std::chrono::seconds>(getTime() - mStartTime).count(); }
        
    private:
        
        std::chrono::steady_clock::time_point getTime() { return mClock.now(); }

        std::chrono::steady_clock mClock;
        std::chrono::steady_clock::time_point mStartTime;
    };
    
    static const int sProcessPerTimeCheck = 200;
    static const int sMaxTime = 5;
    
    /**
     
     @class WorkerThreads
     
     @brief a set of worker threads for processing.
     
     */
    
    class WorkerThreads : public TriggerableThreadSet
    {
        
    public:
        
        WorkerThreads(FrameLib_ProcessingQueue *queue) : TriggerableThreadSet(FrameLib_Thread::kHighPriority, 7), mQueue(queue) {}
        
    private:
        
        virtual void doTask()
        {
            mQueue->mInQueue++;
            mQueue->serviceQueue();
            mQueue->mInQueue--;
        }
        
        FrameLib_ProcessingQueue *mQueue;
    };
    
public:
    
    FrameLib_ProcessingQueue(FrameLib_ErrorReporter& errorReporter)
    : mWorkers(this), mTimedOut(false), mErrorReporter(errorReporter) {}
    
    ~FrameLib_ProcessingQueue()
    {
        mWorkers.join();
    }
    
    // Non-copyable
    
    FrameLib_ProcessingQueue(const FrameLib_ProcessingQueue&) = delete;
    FrameLib_ProcessingQueue& operator=(const FrameLib_ProcessingQueue&) = delete;
    
    void process(FrameLib_DSP *object);
    void start(FrameLib_DSP *object);
    void add(FrameLib_DSP *object);
    void reset() { mTimedOut = false; }
    bool isTimedOut() { return mTimedOut; }

private:
    
    void serviceQueue();

    WorkerThreads mWorkers;
    
    std::atomic<int32_t> mQueueSize;
    std::atomic<int32_t> mInQueue;
    OSFifoQueueHead mQueue;
    
    bool mTimedOut;
    IntervalSecondsClock mClock;
    
    FrameLib_ErrorReporter& mErrorReporter;
};

#endif /* FRAMELIB_PROCESSINGQUEUE_H */
