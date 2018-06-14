
#ifndef FRAMELIB_PROCESSINGQUEUE_H
#define FRAMELIB_PROCESSINGQUEUE_H

#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"

#include <chrono>

// Forward Declarations

class FrameLib_DSP;


// DSP Queue - this allows the DSP network to queue processing with minimal additional data storage (avoiding recursive calls)

class FrameLib_ProcessingQueue
{
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
    
    FrameLib_ProcessingQueue(FrameLib_ErrorReporter& errorReporter) : mTop(NULL), mTail(NULL), mTimedOut(false), mErrorReporter(errorReporter) {}
    
    void add(FrameLib_DSP *object);
    void reset() { mTimedOut = false; }
    bool isTimedOut() { return mTimedOut; }

private:
    
    // Deleted
    
    FrameLib_ProcessingQueue(const FrameLib_ProcessingQueue&);
    FrameLib_ProcessingQueue& operator=(const FrameLib_ProcessingQueue&);
    
    FrameLib_DSP *mTop;
    FrameLib_DSP *mTail;
    
    bool mTimedOut;
    IntervalSecondsClock mClock;
    
    FrameLib_ErrorReporter& mErrorReporter;
};

#endif /* FRAMELIB_PROCESSINGQUEUE_H */
