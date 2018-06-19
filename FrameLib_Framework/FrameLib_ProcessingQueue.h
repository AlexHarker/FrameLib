
#ifndef FRAMELIB_PROCESSINGQUEUE_H
#define FRAMELIB_PROCESSINGQUEUE_H

#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"

#include <chrono>

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
    
public:
    
    FrameLib_ProcessingQueue(FrameLib_ErrorReporter& errorReporter) : mTop(nullptr), mTail(nullptr), mTimedOut(false), mErrorReporter(errorReporter) {}
    
    // Non-copyable
    
    FrameLib_ProcessingQueue(const FrameLib_ProcessingQueue&) = delete;
    FrameLib_ProcessingQueue& operator=(const FrameLib_ProcessingQueue&) = delete;
    
    void add(FrameLib_DSP *object);
    void reset() { mTimedOut = false; }
    bool isTimedOut() { return mTimedOut; }

private:
    
    FrameLib_DSP *mTop;
    FrameLib_DSP *mTail;
    
    bool mTimedOut;
    IntervalSecondsClock mClock;
    
    FrameLib_ErrorReporter& mErrorReporter;
};

#endif /* FRAMELIB_PROCESSINGQUEUE_H */
