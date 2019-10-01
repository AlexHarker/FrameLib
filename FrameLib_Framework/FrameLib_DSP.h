
#ifndef FRAMELIB_DSP_H
#define FRAMELIB_DSP_H

#include "FrameLib_Types.h"
#include "FrameLib_Context.h"
#include "FrameLib_Object.h"
#include "FrameLib_ProcessingQueue.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <vector>

/**
 
 @class FrameLib_DSP
 
 @ingroup DSP

 @brief an abstract class containing the core of the DSP processing system, which handles single-stream scheduling.
 
 */

class FrameLib_DSP : public FrameLib_Block, public FrameLib_Queueable<FrameLib_DSP>
{
    using Queue = FrameLib_Queueable<FrameLib_Block>::Queue;
    using LocalQueue = FrameLib_Queueable<FrameLib_DSP>::Queue;
    using Serial = FrameLib_Parameters::Serial;
    
    friend class FrameLib_ProcessingQueue;
    
protected:
    
    /**
     
     @struct SchedulerInfo
     
     @brief a struct for returning scheduling info from the schedule() method.
     
     */
    
    struct SchedulerInfo
    {
        SchedulerInfo()
        : mTimeAdvance(0), mNewFrame(false), mOutputDone(false) {}
        
        SchedulerInfo(FrameLib_TimeFormat timeAdvance, bool newFrame, bool outputDone)
        : mTimeAdvance(timeAdvance), mNewFrame(newFrame), mOutputDone(outputDone) {}
        
        FrameLib_TimeFormat mTimeAdvance;
        bool mNewFrame;
        bool mOutputDone;
    };
    
private:
    
    /**
     
     @struct Input
     
     @brief a struct that represents an input, its options, connections and any fixed input.
     
     */
    
    struct Input
    {
        Input() : mObject(nullptr), mIndex(0), mSize(0), mFixedInput(nullptr), mType(kFrameNormal), mUpdate(false), mParameters(false), mTrigger(true), mSwitchable(false) {}
        
        FrameType getCurrentType() const { return mObject ? mObject->mOutputs[mIndex].mCurrentType : kFrameNormal; }
        
        // Connection Info
        
        FrameLib_DSP *mObject;
        unsigned long mIndex;
     
        // Fixed Input
        
        unsigned long mSize;
        double *mFixedInput;
        
        // Flags

        FrameType mType;

        bool mUpdate;
        bool mParameters;
        bool mTrigger;
        bool mSwitchable;
    };
   
    /**
     
     @struct Output
     
     @brief a struct that represents an output frame.
     
     */
    
    struct Output
    {
        Output() : mMemory(nullptr), mType(kFrameNormal), mCurrentType(kFrameNormal), mRequestedType(kFrameNormal), mCurrentSize(0), mRequestedSize(0), mPointerOffset(0) {}
        
        void *mMemory;
        
        FrameType mType;
        FrameType mCurrentType;
        FrameType mRequestedType;
        
        unsigned long mCurrentSize;
        unsigned long mRequestedSize;
        size_t mPointerOffset;
    };
    
public:

    using Allocator = FrameLib_Block::Allocator;

    // Constructor / Destructor

    FrameLib_DSP(ObjectType type, FrameLib_Context context, FrameLib_Proxy *proxy, FrameLib_Parameters::Info *info, unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans = 0);
    ~FrameLib_DSP();
    
    // Non-copyable
    
    FrameLib_DSP(const FrameLib_DSP&) = delete;
    FrameLib_DSP& operator=(const FrameLib_DSP&) = delete;

    // Set Fixed Inputs
    
    void setFixedInput(unsigned long idx, const double *input, unsigned long size) final;
    const double *getFixedInput(unsigned long idx, unsigned long *size) final;
    
    // Audio Processing
    
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) final;
    void reset(double samplingRate, unsigned long maxBlockSize) final;
    
    // Info (individual objects should override other methods to provide info)
    
    const FrameLib_Parameters *getParameters() const final { return &mParameters;  }

    FrameType inputType(unsigned long idx) const final  { return mInputs[idx].mType; }
    FrameType outputType(unsigned long idx) const final { return mOutputs[idx].mType; }

    // Automatic Ordering Connections
    
    void autoOrderingConnections() final;
    void clearAutoOrderingConnections() final;

protected:
        
    // Setup and IO Modes
    
    // Call these from your constructor only (unsafe elsewhere)
   
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans = 0);
    void setInputMode(unsigned long idx, bool update, bool trigger, bool switchable, FrameType type = kFrameNormal);
    void setParameterInput(unsigned long idx);
    void addParameterInput();
    void setOutputType(unsigned long idx, FrameType type);

    // Test to see if processing is currently suspended
    
    bool isTimedOut() { return mProcessingQueue->isTimedOut(); }
    
    // You should only call this from your process method (it is unsafe anywhere else)

    void setCurrentOutputType(unsigned long idx, FrameType type);

    // You should only call this from your update method (it is unsafe anywhere else)
    
    void updateTrigger(unsigned long idx, bool trigger);
    
    // Processing Utilities
    
    // Test if an Input Triggered the Current Frame
    
    bool isTrigger(unsigned long idx) const { return mInputs[idx].mTrigger && mInputs[idx].mObject && (mInputs[idx].mObject->mFrameTime == mFrameTime); }
    
    // Timing
    
    FrameLib_TimeFormat getFrameTime() const        { return mFrameTime; }
    FrameLib_TimeFormat getValidTime() const        { return mValidTime; }
    FrameLib_TimeFormat getInputTime() const        { return mInputTime; }
    FrameLib_TimeFormat getCurrentTime() const      { return getType() == kScheduler ? mValidTime : mFrameTime; }
    FrameLib_TimeFormat getBlockStartTime() const   { return getType() == kOutput ? mBlockEndTime : mBlockStartTime; }
    FrameLib_TimeFormat getBlockEndTime() const     { return mBlockEndTime; }
    
    FrameLib_TimeFormat getInputFrameTime(unsigned long idx) const  { return mInputs[idx].mObject ? mInputs[idx].mObject->mFrameTime : FrameLib_TimeFormat(0); }
    FrameLib_TimeFormat getInputValidTime(unsigned long idx) const  { return mInputs[idx].mObject ? mInputs[idx].mObject->mValidTime : FrameLib_TimeFormat(0); }
    
    // Output Allocation
    
    void requestOutputSize(unsigned long idx, unsigned long size)           { mOutputs[idx].mRequestedSize = size; }
    void requestAddedOutputSize(unsigned long idx, unsigned long size)      { mOutputs[idx].mRequestedSize += size; }
    bool allocateOutputs();
    
    // Get Inputs and Outputs
    
    FrameType getInputCurrentType(unsigned long idx) const                          { return mInputs[idx].getCurrentType(); }
    const double *getInput(unsigned long idx, unsigned long *size)  const;
    const FrameLib_Parameters::Serial *getInput(unsigned long idx)  const;
    
    FrameType getOutputCurrentType(unsigned long idx) const                         { return mOutputs[idx].mCurrentType; }
    double *getOutput(unsigned long idx, unsigned long *size)  const;
    FrameLib_Parameters::Serial *getOutput(unsigned long idx)  const;

    // Convience methods for copying and zeroing

    void prepareCopyInputToOutput(unsigned long inIdx, unsigned long outIdx);
    void copyInputToOutput(unsigned long inIdx, unsigned long outIdx);

    static void copyVector(double *output, const double *input, unsigned long size)     { std::copy(input, input + size, output); }
    static void zeroVector(double *output, unsigned long size)                          { std::fill_n(output, size, 0.0); }
    
    static void copyVectorExtend(double* output, const double *input, unsigned long sizeOut, unsigned long sizeIn)
    {
        copyVector(output, input, std::min(sizeIn, sizeOut));
        std::fill_n(output + sizeIn, (sizeOut > sizeIn) ? sizeOut - sizeIn : 0, input[sizeIn - 1]);
    }
    
    static void copyVectorWrap(double* output, const double *input, unsigned long sizeOut, unsigned long sizeIn)
    {
        unsigned long excess = sizeOut % sizeIn;
        
        for (unsigned long i = 0; i < (sizeOut - excess); i += sizeIn)
            copyVector(output + i, input, sizeIn);
        
        copyVector(output + (sizeOut - excess), input, excess);
    }
    
    static void copyVectorZero(double* output, const double *input, unsigned long sizeOut, unsigned long sizeIn)
    {
        copyVector(output, input, std::min(sizeIn, sizeOut));
        zeroVector(output + sizeIn, (sizeOut > sizeIn) ? sizeOut - sizeIn : 0);
    }

    // Convenience methods for converting time values either in time format, or double precision
    
    FrameLib_TimeFormat hzToSamples(const FrameLib_TimeFormat& a) const         { return mSamplingRate / a; }
    FrameLib_TimeFormat msToSamples(const FrameLib_TimeFormat& a) const         { return (a * mSamplingRate) / 1000.0; }
    FrameLib_TimeFormat secondsToSamples(const FrameLib_TimeFormat& a) const    { return a * mSamplingRate; }
    
    double hzToSamples(double a) const          { return mSamplingRate / a; }
    double msToSamples(double a) const          { return (a * mSamplingRate) / 1000.0; }
    double secondsToSamples(double a) const     { return a * mSamplingRate; }

    // Convenience methods for moving from double to int values

    long roundToInt(double a) const             { return static_cast<long>(round(a)); }
    unsigned long roundToUInt(double a) const   { return static_cast<unsigned long>(round(a)); }
    long truncToInt(double a) const             { return static_cast<long>(a); }
    unsigned long truncToUInt(double a) const   { return static_cast<unsigned long>(a); }

private:
    
    // Queueable Reset
    
    void reset(LocalQueue *queue);
    
    // Customisable Processing

    // Override to handle audio at the block level
    
    virtual void blockProcess(const double * const *ins, double **outs, unsigned long blockSize) {}

    // Override to get called on audio reset
    
    virtual void objectReset() {}

    // Override for updates prior to schedule / process (e.g. adjusting triggers)
    
    virtual void update() {}
    
    // Override for scheduling code (scheduler objects must override this)

    virtual SchedulerInfo schedule(bool newFrame, bool noAdvance) = 0;
    
    // Override for main frame processing code (processor objects must override this)

    virtual void process() = 0;
    
    // Scheduling
    
    // This returns true if the object requires notification from an audio thread (is a scheduler/has audio input)
    
    bool requiresAudioNotification()    { return getType() == kScheduler || getNumAudioIns(); }
    
    // Manage Output Memory

    inline void freeOutputMemory();
    inline void releaseOutputMemory();

    // Dependency Notification
    
    inline void dependencyNotify(bool releaseMemory, bool fromInput);
    void dependenciesReady();
    void incrementInputDependency();
    void resetOutputDependencyCount();
    long getNumOuputDependencies()         { return static_cast<long>(mOutputDependencies.size()); }
    
    // Connections
    
    void connectionUpdate(Queue *queue) final;
    void autoOrderingConnections(LocalQueue *queue);

protected:
   
    // Member Variables
    
    // Sampling Rate and Maximum Block Size
    
    double mSamplingRate;
    unsigned long mMaxBlockSize;
    
    // Parameters
    
    FrameLib_Parameters mParameters;

private:
    
    // Processing Queue
    
    FrameLib_Context::ProcessingQueue mProcessingQueue;
    FrameLib_DSP *mNext;
    
    // IO Info
    
    std::vector<FrameLib_DSP *> mInputDependencies;
    std::vector<FrameLib_DSP *> mOutputDependencies;

    std::vector<Input> mInputs;
    std::vector<Output> mOutputs;
    
    // Dependency Counts
    
    long mInputCount;
    long mDependencyCount;
    long mOutputMemoryCount;
    
    // Frame and Block Timings
    
    FrameLib_TimeFormat mFrameTime;
    FrameLib_TimeFormat mValidTime;
    FrameLib_TimeFormat mInputTime;
    FrameLib_TimeFormat mBlockStartTime;
    FrameLib_TimeFormat mBlockEndTime;
    
    bool mUpdatingInputs;
    bool mNoLiveInputs;
    bool mInUpdate;
    bool mOutputDone;
};


/**

 @class FrameLib_Processor

 @ingroup DSP

 @brief a convenience class for creating processor FrameLib_DSP classes that do not handle audio.
 
 Processor classes do not schedule frames and do not handle block-based audio IO. This object provides an empty implementation of schedule() to avoid the end class developer needing to override this method  (never called by a processor class) and also provides a simplified setIO() that does not take a parameter for audio channels.
 
 */

class FrameLib_Processor : public FrameLib_DSP
{
    
public:
    
    FrameLib_Processor(FrameLib_Context context, FrameLib_Proxy *proxy, FrameLib_Parameters::Info *info, unsigned long nIns = 0, unsigned long nOuts = 0)
    : FrameLib_DSP(kProcessor, context, proxy, info, nIns, nOuts) {}
    
    static ObjectType getType() { return kProcessor; }
    static bool handlesAudio()  { return false; }

protected:
    
    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual SchedulerInfo schedule(bool newFrame, bool noAdvance) { return SchedulerInfo(); }
    
    void setIO(unsigned long nIns, unsigned long nOuts) { FrameLib_DSP::setIO(nIns, nOuts); }
};


/**
 
 @class FrameLib_AudioInput

 @ingroup DSP

 @brief a convenience class for creating processor type FrameLib_DSP classes which handle audio input.
 
 Audio input classes do not schedule frames and take block-based audio input (but cannot output audio). This object provides an empty implementation of schedule() to avoid the end class developer needing to override this method (never called by a processor class).
 
 */

class FrameLib_AudioInput : public FrameLib_DSP
{
    
public:
    
    FrameLib_AudioInput(FrameLib_Context context, FrameLib_Proxy *proxy, FrameLib_Parameters::Info *info, unsigned long nIns = 0, unsigned long nOuts = 0, unsigned long nAudioIns = 0)
    : FrameLib_DSP(kProcessor, context, proxy, info, nIns, nOuts, nAudioIns) {}

    static ObjectType getType() { return kProcessor; }
    static bool handlesAudio()  { return true; }
    
protected:
    
    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual SchedulerInfo schedule(bool newFrame, bool noAdvance) { return SchedulerInfo(); }
};


/**
 
 @class FrameLib_AudioOutput
 
 @ingroup DSP

 @brief a convenience class for creating processor type FrameLib_DSP classes which handle audio output.
 
 Audio output classes do not schedule frames and create block-based audio output (but not audio input). This object provides an empty implementation of schedule() to avoid the end class developer needing to override this method (never called by a processor class).
 
 */

class FrameLib_AudioOutput : public FrameLib_DSP
{
    
public:
    
    FrameLib_AudioOutput(FrameLib_Context context, FrameLib_Proxy *proxy, FrameLib_Parameters::Info *info, unsigned long nIns = 0, unsigned long nOuts = 0, unsigned long nAudioOuts = 0)
    : FrameLib_DSP(kOutput, context, proxy, info, nIns, nOuts, nAudioOuts) {}
    
    static ObjectType getType() { return kOutput; }
    static bool handlesAudio()  { return true; }
    
protected:
    
    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual SchedulerInfo schedule(bool newFrame, bool noAdvance) { return SchedulerInfo(); }
};


/**
 
 @class FrameLib_Scheduler

 @ingroup DSP

 @brief a convenience class for creating scheduler type FrameLib_DSP classes.
 
 Scheduler classes schedule frames and may deal with  block-based audio input (but not audio ioutput). This object provides an empty implementation of process() to avoid the end class developer needing to override this method (never called by a scheduler class).
 
 */

class FrameLib_Scheduler : public FrameLib_DSP
{

public:
    
    FrameLib_Scheduler(FrameLib_Context context, FrameLib_Proxy *proxy, FrameLib_Parameters::Info *info, unsigned long nIns = 0, unsigned long nOuts = 0, unsigned long nAudioIns = 0)
    : FrameLib_DSP(kScheduler, context, proxy, info, nIns, nOuts, nAudioIns) {}
    
    static ObjectType getType() { return kScheduler; }
    static bool handlesAudio()  { return true; }
    
protected:

    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual void process() {}
};

#endif
