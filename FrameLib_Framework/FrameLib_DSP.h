
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
        
    // ************************************************************************************** //

    // Enums and Structs (IO / scheduling)
    
public:

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
    
    // ************************************************************************************** //

    // Constructor / Destructor
    
public:
    
    FrameLib_DSP(ObjectType type, FrameLib_Context context, unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns, unsigned long nAudioOuts);
    ~FrameLib_DSP();
    
   // Basic Setup
    
    void setSamplingRate(double samplingRate)   { mSamplingRate = samplingRate > 0 ? samplingRate : 44100.0; }
    
    virtual unsigned long getNumIns()           { return mInputs.size(); }
    virtual unsigned long getNumOuts()          { return mOutputs.size(); }
    virtual unsigned long getNumAudioIns()      { return mNumAudioIns; }
    virtual unsigned long getNumAudioOuts()     { return mNumAudioOuts; }
    
    // Object Type
    
protected:
    
    ObjectType getType()    { return mType; }

    // ************************************************************************************** //
    
    // IO Utilities
    
protected:
    
    // Call this in derived class constructors only if the IO size is not fixed
   
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns = 0, unsigned long nAudioOuts = 0);
    
    
    // ************************************************************************************** //
    
    // Input/Output Modes
    
protected:
    
    // Call this from your constructor only (unsafe elsewhere)
    
    void inputMode(unsigned long idx, bool update, bool trigger, bool switchable);
    
    // You should only call this from your update method (it is unsafe anywhere else)
    
    void updateTrigger(unsigned long idx, bool trigger);
    
    // Call this from your constructor only (unsafe elsewhere)
    
    void outputMode(unsigned long idx, OutputMode mode);

    // ************************************************************************************** //
    
    // Processing methods
 
private:
    
    // This returns true if the object requires notification from an audio thread
    
    bool requiresAudioNotification()
    {
        // FIX- review this...
        
        return mType == kScheduler || getNumAudioIns() || getNumAudioOuts();
    }
    
    // Block updates for objects with audio IO
    
    void blockUpdate(double **ins, double **outs, unsigned long vecSize);
    
    // Dependency notification
    
    inline void notify(bool releaseMemory);
    
    // Release output memory
    
    inline void releaseOutputMemory();
    
    // Main code to control time flow (called when all input/output dependencies are ready)
    
    void dependenciesReady();
    
    // ************************************************************************************** //

    // Resets
    
private:
    
    inline void freeOutputMemory();
    void resetDependencyCount();
    
public:

    void reset();
    void setFixedInput(unsigned long idx, double *input, unsigned long size);
    
    // ************************************************************************************** //
    
    // Processing utilities
    
protected:
    
    bool isTrigger(unsigned long idx)
    {
        return mInputs[idx].mTrigger && mInputs[idx].mObject && (mInputs[idx].mObject->mFrameTime == mFrameTime);
    }
    
    FrameLib_TimeFormat getFrameTime()      { return mFrameTime; }
    FrameLib_TimeFormat getValidTime()      { return mValidTime; }
    FrameLib_TimeFormat getBlockStartTime() { return mBlockStartTime; }
    FrameLib_TimeFormat getBlockEndTime()   { return mBlockEndTime; }
    
    FrameLib_TimeFormat getInputFrameTime(unsigned long idx)        { return mInputs[idx].mObject ? mInputs[idx].mObject->mFrameTime : 0.0; }
    FrameLib_TimeFormat getInputValidTillTime(unsigned long idx)    { return mInputs[idx].mObject ? mInputs[idx].mObject->mValidTime : 0.0; }
    
    void requestOutputSize(unsigned long idx, size_t size)          { mOutputs[idx].mRequestedSize = size; }
    
    bool allocateOutputs();
    
    double *getOutput(unsigned long idx, size_t *size);
    FrameLib_Attributes::Serial *getOutput(unsigned long idx);

    double *getInput(unsigned long idx, size_t *size);
    FrameLib_Attributes::Serial *getInput(unsigned long idx);
    
    // Customisable processing
    
private:
    
    // Override to handle audio at the block level (objects with block-based audio must overload this)
    
    virtual void blockProcessPre(double **ins, double **outs, unsigned long vecSize) {}
    virtual void blockProcessPost(double **ins, double **outs, unsigned long vecSize) {}
    
    // Override for updates prior to schedule / process (e.g. adjusting triggers)
    
    virtual void update(){}
    
    // Override for scheduling code (scheduler objects must override this)

    virtual SchedulerInfo schedule(bool newFrame, bool noOutput) = 0;
    
    // Override for main frame processing code (processor objects must override this)

    virtual void process() = 0;

    // ************************************************************************************** //

    // Connection mathods (private)
    
private:
    
    // Dependency updating
    
    std::vector <FrameLib_DSP *>::iterator removeInputDependency(FrameLib_DSP *object);
    std::vector <FrameLib_DSP *>::iterator removeOutputDependency(FrameLib_DSP *object);
    void addInputDependency(FrameLib_DSP *object);
    void addOutputDependency(FrameLib_DSP *object);
    
    // Removal of one connection to this object (before replacement / deletion)

    void removeConnection(unsigned long inIdx);
    
    // Removal of all connections from one object to this object
    
    std::vector <FrameLib_DSP *>::iterator removeConnections(FrameLib_DSP *object);
   
    // ************************************************************************************** //
    
    // Connection methods (public)
    
public:
    
    // N.B. - No sanity checks here to maximise speed and help debugging (better for it to crash if a mistake is made)
    
    void deleteConnection(unsigned long inIdx);
    void addConnection(FrameLib_DSP *object, unsigned long outIdx, unsigned long inIdx);
    void clearConnections();
    virtual bool isConnected(unsigned long inIdx);
    
protected:
    
    FrameLib_DSP *getOutputObject(unsigned long outIdx)
    {
        return this;
    }
    
    // ************************************************************************************** //
    
    // Member variables
   
protected:
    
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
    
    virtual SchedulerInfo schedule(bool newFrame, bool noOutput)    { return SchedulerInfo(); }
    
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
    
    virtual void process(){}
};

#endif

