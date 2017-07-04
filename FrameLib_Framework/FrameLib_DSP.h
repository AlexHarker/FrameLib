
#ifndef FRAMELIB_DSP_H
#define FRAMELIB_DSP_H

#include "FrameLib_Types.h"
#include "FrameLib_Context.h"
#include "FrameLib_Block.h"
#include "FrameLib_DSPQueue.h"
#include <limits>
#include <vector>

// FrameLib_DSP

// This abstract class is the core of the DSP processing system and handles low level single channel connections and timing

class FrameLib_DSP : public FrameLib_Block
{
    typedef FrameLib_Attributes::Serial Serial;
    friend FrameLib_DSPQueue;

public:
    
    // Enums and Structs (IO / scheduling)
    

    enum ObjectType { kProcessor, kScheduler };
    enum OutputMode { kOutputNormal, kOutputTagged };
    
protected:
    
    struct SchedulerInfo
    {
        SchedulerInfo()
        : mTimeAdvance(), mNewFrame(), mOutputDone() {}
        
        SchedulerInfo(FrameLib_TimeFormat timeAdvance, bool newFrame, bool outputDone)
        : mTimeAdvance(timeAdvance), mNewFrame(newFrame), mOutputDone(outputDone) {}
        
        FrameLib_TimeFormat mTimeAdvance;
        bool mNewFrame;
        bool mOutputDone;
    };
    
private:
    
    struct Input
    {
        Input() : mObject(NULL), mIndex(0), mSize(0), mFixedInput(NULL), mUpdate(false), mTrigger(true), mSwitchable(false) {}
        
        void SetInput()
        {
            mObject = NULL;
            mIndex = 0;
        }
        
        void SetInput(FrameLib_DSP *object, unsigned long idx)
        {
            mObject = object;
            mIndex = idx;
        }
        
        // Connection Info
        
        FrameLib_DSP *mObject;
        unsigned long mIndex;
     
        // Fixed Input
        
        unsigned long mSize;
        double *mFixedInput;
        
        // Flags
        
        bool mUpdate;
        bool mTrigger;
        bool mSwitchable;
    };
   
    struct Output
    {
        Output() : mMemory(NULL), mMode(kOutputNormal), mCurrentSize(0), mRequestedSize(0), mPointerOffset(0) {}
        
        void *mMemory;
        
        OutputMode mMode;
        
        size_t mCurrentSize;
        size_t mRequestedSize;
        size_t mPointerOffset;
    };
    
public:

    // Constructor / Destructor

    FrameLib_DSP(ObjectType type, FrameLib_Context context, unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns, unsigned long nAudioOuts);
    ~FrameLib_DSP();
    
   // Basic Setup
    
    void setSamplingRate(double samplingRate)   { mSamplingRate = samplingRate > 0 ? samplingRate : 44100.0; }
    
    virtual unsigned long getNumIns()           { return mInputs.size(); }
    virtual unsigned long getNumOuts()          { return mOutputs.size(); }
    virtual unsigned long getNumAudioIns()      { return mNumAudioIns; }
    virtual unsigned long getNumAudioOuts()     { return mNumAudioOuts; }
    
    // Set Fixed Inputs
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size);

    // Audio Processing
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long vecSize);
    virtual void reset();
    
    // Connection Methods
    
    // N.B. - No sanity checks here to maximise speed / help debugging (better for it to crash if a mistake is made)
    
    virtual void deleteConnection(unsigned long inIdx);
    virtual void addConnection(FrameLib_DSP *object, unsigned long outIdx, unsigned long inIdx);
    virtual void clearConnections();
    virtual bool isConnected(unsigned long inIdx);
    
protected:
    
    // Object Type

    ObjectType getType()    { return mType; }
    
    // Setup and IO Modes
    
    // Call these from your constructor only (unsafe elsewhere)
   
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns = 0, unsigned long nAudioOuts = 0);
    void inputMode(unsigned long idx, bool update, bool trigger, bool switchable);
    void outputMode(unsigned long idx, OutputMode mode);
    
    // You should only call this from your update method (it is unsafe anywhere else)
    
    void updateTrigger(unsigned long idx, bool trigger);
    
    // Processing Utilities
    
    // Test if an Input Triggered the Current Frame
    
    bool isTrigger(unsigned long idx) { return mInputs[idx].mTrigger && mInputs[idx].mObject && (mInputs[idx].mObject->mFrameTime == mFrameTime); }
    
    // Timing
    
    FrameLib_TimeFormat getFrameTime()      { return mFrameTime; }
    FrameLib_TimeFormat getValidTime()      { return mValidTime; }
    FrameLib_TimeFormat getBlockStartTime() { return mBlockStartTime; }
    FrameLib_TimeFormat getBlockEndTime()   { return mBlockEndTime; }
    
    FrameLib_TimeFormat getInputFrameTime(unsigned long idx)        { return mInputs[idx].mObject ? mInputs[idx].mObject->mFrameTime : 0.0; }
    FrameLib_TimeFormat getInputValidTillTime(unsigned long idx)    { return mInputs[idx].mObject ? mInputs[idx].mObject->mValidTime : 0.0; }
    
    // Output Allocation
    
    void requestOutputSize(unsigned long idx, size_t size)          { mOutputs[idx].mRequestedSize = size; }
    bool allocateOutputs();
    
    // Get Inputs and Outputs
    
    double *getInput(unsigned long idx, size_t *size);
    FrameLib_Attributes::Serial *getInput(unsigned long idx);
    
    double *getOutput(unsigned long idx, size_t *size);
    FrameLib_Attributes::Serial *getOutput(unsigned long idx);

    // Get DSP Object for a Given Output
    
    FrameLib_DSP *getOutputObject(unsigned long outIdx)     { return this; }
    
private:
    
    // Customisable Processing

    // Override to handle audio at the block level (pre or post processing by the host)
    
    virtual void blockProcessPre(double **ins, double **outs, unsigned long vecSize) {}
    virtual void blockProcessPost(double **ins, double **outs, unsigned long vecSize) {}

    // Override for updates prior to schedule / process (e.g. adjusting triggers)
    
    virtual void update() {}
    
    // Override for scheduling code (scheduler objects must override this)

    virtual SchedulerInfo schedule(bool newFrame, bool noOutput) = 0;
    
    // Override for main frame processing code (processor objects must override this)

    virtual void process() = 0;
    
    // Scheduling
    
    // This returns true if the object requires notification from an audio thread
    
    bool requiresAudioNotification()    { return mType == kScheduler || getNumAudioIns() || getNumAudioOuts(); }
    
    // Manage Output Memory

    inline void freeOutputMemory();
    inline void releaseOutputMemory();

    // Dependency Notification
    
    inline void dependencyNotify(bool releaseMemory);
    void dependenciesReady();
    void resetDependencyCount();
    
    // Dependency Updating
    
    std::vector <FrameLib_DSP *>::iterator removeInputDependency(FrameLib_DSP *object);
    std::vector <FrameLib_DSP *>::iterator removeOutputDependency(FrameLib_DSP *object);
    void addInputDependency(FrameLib_DSP *object);
    void addOutputDependency(FrameLib_DSP *object);
    
    // Connection Methods (private)

    void clearConnection(unsigned long inIdx);
    void removeConnection(unsigned long inIdx);
    std::vector <FrameLib_DSP *>::iterator disconnect(FrameLib_DSP *object);
    
protected:
   
    // Member Variables

    // Context
    
    FrameLib_Context mContext;
    
    // Sampling Rate
    
    double mSamplingRate;
    
    // Memory Allocator
    
    FrameLib_LocalAllocator *mAllocator;
    
private:
    
    // DSP Queue
    
    FrameLib_DSPQueue *mQueue;
    FrameLib_DSP *mNext;
    
    // Object Type
    
    const ObjectType mType;
    
    // IO Info
    
    std::vector <Input> mInputs;
    std::vector <Output> mOutputs;
    
    std::vector <FrameLib_DSP *> mInputDependencies;
    std::vector <FrameLib_DSP *> mOutputDependencies;
    
    // Audio IO Counts
    
    unsigned long mNumAudioIns;
    unsigned long mNumAudioOuts;
    
    // Dependency Counts
    
    long mDependencyCount;
    long mOutputMemoryCount;
    
    // Frame and Block Timings
    
    FrameLib_TimeFormat mFrameTime;
    FrameLib_TimeFormat mValidTime;
    FrameLib_TimeFormat mInputTime;
    FrameLib_TimeFormat mBlockStartTime;
    FrameLib_TimeFormat mBlockEndTime;
    
    bool mInUpdate;
    bool mOutputDone;
};

// ************************************************************************************** //

// FrameLib_Processor - Simple class for process type objects (can't handle audio)

class FrameLib_Processor : public FrameLib_DSP
{
    
public:
    
    FrameLib_Processor(FrameLib_Context context, unsigned long nIns = 0, unsigned long nOuts = 0)
    : FrameLib_DSP(kProcessor, context, nIns, nOuts, 0, 0) {}
    
    static bool handlesAudio() { return false; }

protected:
    
    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual SchedulerInfo schedule(bool newFrame, bool noOutput) { return SchedulerInfo(); }
    
    void setIO(unsigned long nIns, unsigned long nOuts) { FrameLib_DSP::setIO(nIns, nOuts); }
};

// ************************************************************************************** //

// FrameLib_AudioProcessor - Simple class for process type objects (can handle audio)

class FrameLib_AudioProcessor : public FrameLib_DSP
{
    
public:
    
    FrameLib_AudioProcessor(FrameLib_Context context, unsigned long nIns = 0, unsigned long nOuts = 0, unsigned long nAudioIns = 0, unsigned long nAudioOuts = 0)
    : FrameLib_DSP(kProcessor, context, nIns, nOuts, nAudioIns, nAudioOuts) {}
    
    static bool handlesAudio() { return true; }
    
protected:
    
    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual SchedulerInfo schedule(bool newFrame, bool noOutput)    { return SchedulerInfo(); }
};

// ************************************************************************************** //

// FrameLib_Scheduler - Simple class for scheduler type objects

class FrameLib_Scheduler : public FrameLib_DSP
{

public:
    
    FrameLib_Scheduler(FrameLib_Context context, unsigned long nIns = 0, unsigned long nOuts = 0, unsigned long nAudioIns = 0, unsigned long nAudioOuts = 0)
    : FrameLib_DSP(kScheduler, context, nIns, nOuts, nAudioIns, nAudioOuts) {}
    
    static bool handlesAudio() { return true; }
    
protected:

    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual void process() {}
};

#endif
