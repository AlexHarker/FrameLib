
#ifndef FRAMELIB_PROCESSINGQUEUE_H
#define FRAMELIB_PROCESSINGQUEUE_H

#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"
#include "FrameLib_Memory.h"
#include "FrameLib_Queues.h"
#include "FrameLib_Threading.h"

#include <chrono>
#include <vector>

#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
#include <xmmintrin.h>
#endif

// Forward Declarations

class FrameLib_Global;
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
public:
    
    using MainQueue = FrameLib_LockFreeStack<FrameLib_DSP>;
    using PrepQueue = MainQueue::Queue;
    using MainNode = MainQueue::Node;
    using ThreadNode = FrameLib_Node<FrameLib_DSP, FrameLib_ProcessingQueue>;

    struct Node : MainNode, ThreadNode {};

    /**
     
     @class IntervalMicosecondsClock
     
     @brief a clock for measuring time intervals in seconds.
     
     */

    class IntervalMicrosecondsClock
    {
    public:
        
        void start() { mStartTime = getTime(); }
        long long elapsed() { return std::chrono::duration_cast<std::chrono::microseconds>(getTime() - mStartTime).count(); }
        
    private:
        
        std::chrono::steady_clock::time_point getTime() { return mClock.now(); }

        std::chrono::steady_clock mClock;
        std::chrono::steady_clock::time_point mStartTime;
    };
    
    static constexpr int processPerTimeCheck = 200;
    
    /**
     
     @class WorkerThreads
     
     @brief a set of worker threads for processing.
     
     */
    
    class WorkerThreads final : public FrameLib_TriggerableThreadSet
    {
        static unsigned int numThreads() { return FrameLib_Thread::maxThreads() - 1; }
        
    public:
        
        WorkerThreads(FrameLib_ProcessingQueue *queue)
        : FrameLib_TriggerableThreadSet(FrameLib_Thread::PriorityLevel::Audio, numThreads())
        , mQueue(queue)
        {}
        
    private:
        
        void doTask(unsigned int index) override;

        FrameLib_ProcessingQueue *mQueue;
    };
    
public:
    
    // Constructor / Destructor
    
    FrameLib_ProcessingQueue(FrameLib_Global& global);
    ~FrameLib_ProcessingQueue();
    
    // Non-copyable
    
    FrameLib_ProcessingQueue(const FrameLib_ProcessingQueue&) = delete;
    FrameLib_ProcessingQueue& operator=(const FrameLib_ProcessingQueue&) = delete;
    
    // Start and add items to the queue
    
    void start(PrepQueue &queue);
    void add(PrepQueue &queue, FrameLib_DSP *addedBy);

    // Additional functionality
    
    void reset() { mTimedOut = false; }
    bool isTimedOut() { return mTimedOut; }
    void setTimeOuts(double relative = 0.0, double absolute = 0.0);
    void setMultithreading(bool multithread) { mMultithread = multithread; }
    
    // Timing info
    
    FrameLib_TimeFormat getBlockStartTime() const;
    FrameLib_TimeFormat getBlockEndTime() const;
    
private:
    
    void enqueue(PrepQueue &queue);
    void wakeWorkers();
    void serviceQueue(FrameLib_LocalAllocator *allocator);
    void calculateTimeOutMax();
    bool checkForTimeOut();
    
    WorkerThreads mWorkers;
    FrameLib_LocalAllocatorSet mAllocators;

    MainQueue mQueue;
    
    std::atomic<int32_t> mNumItems;
    std::atomic<int32_t> mNumWorkersActive;
    
    bool mMultithread;
    long long mMaxTime;
    double mRelativeTimeOut;
    double mAbsoluteTimeOut;
    std::atomic<bool> mTimedOut;
    IntervalMicrosecondsClock mClock;
    
    FrameLib_DSP *mEntryObject;
    FrameLib_ErrorReporter& mErrorReporter;
};


/**
 
 @class FrameLib_AudioQueue
 
 @ingroup ProcessingQueue
 
 @brief a small queue that is used to combine multiple audio block notifications to a single call to the main processing queue.
 
 */

class FrameLib_AudioQueue : private FrameLib_ProcessingQueue::PrepQueue
{
    friend class FrameLib_DSP;
    
public:
    
    FrameLib_AudioQueue() : mUser(nullptr) {}
    ~FrameLib_AudioQueue(); 
    
private:
    
    void setUser(FrameLib_DSP *object) { mUser = object; }
    
    FrameLib_DSP *mUser;
};

#endif /* FRAMELIB_PROCESSINGQUEUE_H */
