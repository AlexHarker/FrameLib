
#ifndef FRAMELIB_DSP_H
#define FRAMELIB_DSP_H

#include "FrameLib_Types.h"
#include "FrameLib_Context.h"
#include "FrameLib_Object.h"
#include "FrameLib_ProcessingQueue.h"
#include <limits>
#include <vector>
#include <algorithm>

// FrameLib_DSP

// This abstract class is the core of the DSP processing system and handles low level single channel connections and timing

class FrameLib_DSP : public FrameLib_Block, public FrameLib_Queueable<FrameLib_DSP>
{
    // Type definition for concision / Queue access

    typedef FrameLib_Queueable<FrameLib_Block>::Queue Queue;
    typedef FrameLib_Queueable<FrameLib_DSP>::Queue LocalQueue;
    typedef FrameLib_Parameters::Serial Serial;

    friend class FrameLib_ProcessingQueue;
    
protected:
    
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
    
    struct Input
    {
        Input() : mObject(NULL), mIndex(0), mSize(0), mFixedInput(NULL), mType(kFrameNormal), mUpdate(false), mParameters(false), mTrigger(true), mSwitchable(false) {}
        
        FrameType getCurrentType() { return mObject ? mObject->mOutputs[mIndex].mCurrentType : kFrameNormal; }
        
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
   
    struct Output
    {
        Output() : mMemory(NULL), mType(kFrameNormal), mCurrentType(kFrameNormal), mRequestedType(kFrameNormal), mCurrentSize(0), mRequestedSize(0), mPointerOffset(0) {}
        
        void *mMemory;
        
        FrameType mType;
        FrameType mCurrentType;
        FrameType mRequestedType;
        
        size_t mCurrentSize;
        size_t mRequestedSize;
        size_t mPointerOffset;
    };
    
    // Typedefs for concision
    
    typedef std::vector<Input>::iterator InputIterator;
    typedef std::vector<Output>::iterator OutputIterator;
    typedef std::vector<FrameLib_DSP *>::iterator ObjectIterator;

public:

    // Constructor / Destructor

    FrameLib_DSP(ObjectType type, FrameLib_Context context, void *owner, FrameLib_Parameters::Info *info, unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans = 0);
    ~FrameLib_DSP();
    
    // Set Fixed Inputs
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size);

    // Audio Processing
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long blockSize);
    virtual void reset(double samplingRate, unsigned long maxBlockSize);
    
    // Info (individual objects should override other methods to provide info)
    
    virtual const FrameLib_Parameters *getParameters() const { return &mParameters;  }

    virtual FrameType inputType(unsigned long idx) const  { return mInputs[idx].mType; }
    virtual FrameType outputType(unsigned long idx) const { return mOutputs[idx].mType; }

    // Automatic Oredering Connections
    
    virtual void autoOrderingConnections();
    virtual void clearAutoOrderingConnections();

protected:
        
    // Setup and IO Modes
    
    // Call these from your constructor only (unsafe elsewhere)
   
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans = 0);
    void setInputMode(unsigned long idx, bool update, bool trigger, bool switchable, FrameType type = kFrameNormal);
    void setParameterInput(unsigned long idx);
    void addParameterInput();
    void setOutputMode(unsigned long idx, FrameType type);

    // You should only call this from your process method (it is unsafe anywhere else)

    void setCurrentOutputMode(unsigned long idx, FrameType type);

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
    
    void requestOutputSize(unsigned long idx, size_t size)          { mOutputs[idx].mRequestedSize = size; }
    bool allocateOutputs();
    
    // Get Inputs and Outputs
    
    double *getInput(unsigned long idx, size_t *size);
    FrameLib_Parameters::Serial *getInput(unsigned long idx);
    
    double *getOutput(unsigned long idx, size_t *size);
    FrameLib_Parameters::Serial *getOutput(unsigned long idx);

    // Convience methods for copying and zeroing

    void prepareCopyInputToOutput(unsigned long inIdx, unsigned long outIdx);
    void copyInputToOutput(unsigned long inIdx, unsigned long outIdx);

    static void copyVector(double *output, double *input, unsigned long size)      { std::copy(input, input + size, output); }
    static void zeroVector(double *output, unsigned long size)                     { std::fill_n(output, size, 0.0); }
    
private:
    
    // Deleted
    
    FrameLib_DSP(const FrameLib_DSP&);
    FrameLib_DSP& operator=(const FrameLib_DSP&);

    // Queueable Reset
    
    void reset(LocalQueue *queue);
    
    // Customisable Processing

    // Override to handle audio at the block level
    
    virtual void blockProcess(double **ins, double **outs, unsigned long blockSize) {}

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
    void setOutputDependencyCount();
    void incrementInputDependency();
    
    // Connections
    
    virtual void connectionUpdate(Queue *queue);
    virtual void autoOrderingConnections(LocalQueue *queue);

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

    std::vector <Input> mInputs;
    std::vector <Output> mOutputs;
    
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

// ************************************************************************************** //

// FrameLib_Processor - Simple class for process type objects (can't handle audio)

class FrameLib_Processor : public FrameLib_DSP
{
    
public:
    
    FrameLib_Processor(FrameLib_Context context, void *owner, FrameLib_Parameters::Info *info, unsigned long nIns = 0, unsigned long nOuts = 0)
    : FrameLib_DSP(kProcessor, context, owner, info, nIns, nOuts) {}
    
    static ObjectType getType() { return kProcessor; }
    static bool handlesAudio()  { return false; }

protected:
    
    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual SchedulerInfo schedule(bool newFrame, bool noAdvance) { return SchedulerInfo(); }
    
    void setIO(unsigned long nIns, unsigned long nOuts) { FrameLib_DSP::setIO(nIns, nOuts); }
};

// ************************************************************************************** //

// FrameLib_AudioInput - Simple class for process type objects (can handle audio input)

class FrameLib_AudioInput : public FrameLib_DSP
{
    
public:
    
    FrameLib_AudioInput(FrameLib_Context context, void *owner, FrameLib_Parameters::Info *info, unsigned long nIns = 0, unsigned long nOuts = 0, unsigned long nAudioIns = 0)
    : FrameLib_DSP(kProcessor, context, owner, info, nIns, nOuts, nAudioIns) {}

    static ObjectType getType() { return kProcessor; }
    static bool handlesAudio()  { return true; }
    
protected:
    
    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual SchedulerInfo schedule(bool newFrame, bool noAdvance) { return SchedulerInfo(); }
};

// ************************************************************************************** //

// FrameLib_AudioOutput - Simple class for process type objects (can handle audio output)

class FrameLib_AudioOutput : public FrameLib_DSP
{
    
public:
    
    FrameLib_AudioOutput(FrameLib_Context context, void *owner, FrameLib_Parameters::Info *info, unsigned long nIns = 0, unsigned long nOuts = 0, unsigned long nAudioOuts = 0)
    : FrameLib_DSP(kOutput, context, owner, info, nIns, nOuts, nAudioOuts) {}
    
    static ObjectType getType() { return kOutput; }
    static bool handlesAudio()  { return true; }
    
protected:
    
    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual SchedulerInfo schedule(bool newFrame, bool noAdvance) { return SchedulerInfo(); }
};

// ************************************************************************************** //

// FrameLib_Scheduler - Simple class for scheduler type objects

class FrameLib_Scheduler : public FrameLib_DSP
{

public:
    
    FrameLib_Scheduler(FrameLib_Context context, void *owner, FrameLib_Parameters::Info *info, unsigned long nIns = 0, unsigned long nOuts = 0, unsigned long nAudioIns = 0)
    : FrameLib_DSP(kScheduler, context, owner, info, nIns, nOuts, nAudioIns) {}
    
    static ObjectType getType() { return kScheduler; }
    static bool handlesAudio()  { return true; }
    
protected:

    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual void process() {}
};

#endif
