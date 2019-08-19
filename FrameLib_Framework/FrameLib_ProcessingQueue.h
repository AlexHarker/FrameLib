
#ifndef FRAMELIB_PROCESSINGQUEUE_H
#define FRAMELIB_PROCESSINGQUEUE_H

#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"
#include "FrameLib_Memory.h"
#include "FrameLib_Threading.h"

#include <chrono>
#include <vector>

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
    
    /**
     
     @class Node
     
     @brief a node in the processing queue.
     
     */
    
    struct Node
    {
        /**
         
         @class Pointer
         
         @brief a pointer with a count attached
         
         */
        
        struct Pointer
        {
            Pointer() : mPointer(nullptr), mCount(0) {}
            Pointer(Node *node, uintptr_t count) : mPointer(node), mCount(count) {}
            
            bool operator==(const Pointer& a) const
            {
                return a.mPointer == mPointer && a.mCount == mCount;
            }
            
            // This structure must not have padding bits, so we use a pointer sized mCount
            
            Node *mPointer;
            uintptr_t mCount;
        };
        
        Node(FrameLib_DSP *owner)
        : mOwner(owner), mNextInThread(nullptr), mNext(Pointer()) {}
        
        FrameLib_DSP *mOwner;
        FrameLib_DSP *mNextInThread;
        std::atomic<Pointer> mNext;
    };
    
private:
    
    using NodePointer = Node::Pointer;

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
    
    class WorkerThreads : public FrameLib_TriggerableThreadSet
    {
        
    public:
        
        WorkerThreads(FrameLib_ProcessingQueue *queue)
        : FrameLib_TriggerableThreadSet(FrameLib_Thread::kAudioPriority, FrameLib_Thread::maxThreads() - 1), mQueue(queue)
        {}
        
    private:
        
        void doTask(unsigned int index) override { mQueue->serviceQueue(index + 1); }

        FrameLib_ProcessingQueue *mQueue;
    };
    
public:
    
    // Constructor / Destructor
    
    FrameLib_ProcessingQueue(FrameLib_Global& global);
    ~FrameLib_ProcessingQueue();
    
    // Non-copyable
    
    FrameLib_ProcessingQueue(const FrameLib_ProcessingQueue&) = delete;
    FrameLib_ProcessingQueue& operator=(const FrameLib_ProcessingQueue&) = delete;
    
    void process(FrameLib_DSP *object);
    void add(FrameLib_DSP *object, FrameLib_DSP *addedBy);
    void reset() { mTimedOut = false; }
    bool isTimedOut() { return mTimedOut; }

private:
    
    void serviceQueue(int32_t index);
    
    void init();
    void enqueue(FrameLib_DSP *object);
    FrameLib_DSP *dequeue();
    
    WorkerThreads mWorkers;
    FrameLib_OwnedList<FrameLib_FreeBlocks> mFreeBlocks;

    Node mDummyNode;
    std::atomic<NodePointer> mHead;
    std::atomic<NodePointer> mTail;
    
    std::atomic<int32_t> mNumItems;
    std::atomic<int32_t> mNumWorkersActive;
    
    bool mTimedOut;
    IntervalSecondsClock mClock;
    
    FrameLib_ErrorReporter& mErrorReporter;
};

#endif /* FRAMELIB_PROCESSINGQUEUE_H */
