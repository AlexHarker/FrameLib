
#ifndef FRAMELIB_DSP_H
#define FRAMELIB_DSP_H

#include "FrameLib_Types.h"
#include "FrameLib_Block.h"
#include "FrameLib_Memory.h"
#include <limits>
#include <vector>

// FrameLib_DSP

// This abstract class is the core of the DSP processing system and handles low level single channel connections and timing

class FrameLib_DSP : public FrameLib_Block
{
    
private:
    
    static const size_t taggedOutputAlignment = FrameLib_Attributes::Serial::alignment;
    
    // ************************************************************************************** //

    // DSP Queue - this allows the DSP network to queue processing with minimal additional data storage (avoiding recursive calls)
    
public:
    
    class DSPQueue
    {
        
    public:
        
        DSPQueue() : mTop(NULL), mTail(NULL) {}
        
        void add(FrameLib_DSP *object)
        {
            object->mNext = NULL;
            
            if (!mTop)
            {
                // Queue is empty - add and start processing the queue
                
                mTop = mTail = object;
                
                while (mTop)
                {
                    object = mTop;
                    object->dependenciesReady();
                    mTop = object->mNext;
                    object->mNext = NULL;
                }
                
                mTail = NULL;
            }
            else
            {
                // Add to the queue (which is already processing)
                
                mTail->mNext = object;
                mTail = object;
            }
        }
        
    private:

        FrameLib_DSP *mTop;
        FrameLib_DSP *mTail;
    };

    // ************************************************************************************** //

    // Enums and Structs (IO / scheduling)
    
public:

    enum ObjectType { kProcessor, kOutput, kScheduler };
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
        Input() : mObject(NULL), mIndex(0), mUpdate(FALSE), mTrigger(TRUE), mSwitchable(FALSE) {}
        
        // Connection Info
        
        FrameLib_DSP *mObject;
        unsigned long mIndex;
     
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

    // Constructor
    
public:
    
    FrameLib_DSP (ObjectType type, DSPQueue *queue, FrameLib_Memory *allocator, unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns, unsigned long nAudioOuts)
    : FrameLib_Block(nAudioIns, nAudioOuts), mType(type)
    {
        // Resize inputs and outputs
        
        mInputs.resize((mType != kScheduler || nIns > 0) ? nIns : 1);
        mOutputs.resize(nOuts);
        
        // Store Queue and Allocator
        
        mQueue = queue;
        mNext = NULL;
        mAllocator = allocator;
        
        // Reset for audio
        
        reset();
    }
    
    // Destructor
    
    ~FrameLib_DSP ()
    {
        // Clear connections before deleting
        
        clearConnections();
    }
    
    // ************************************************************************************** //

    // Copy Constructor

    FrameLib_DSP (FrameLib_DSP &object) : FrameLib_Block(object.getNumAudioIns(), object.getNumAudioIns()), mType(object.mType)
    {
        // Resize inputs and outputs
        
        mInputs.resize(object.getNumIns());
        mOutputs.resize(object.getNumOuts());
        
        // N.B. - copying of input modes must be done by inheriting objects
        
        // Store Queue and Allocator
        
        mQueue = object.mQueue;
        mNext = NULL;
        mAllocator = object.mAllocator;
        
        // Reset for audio
        
        reset();
    }
    
    // ************************************************************************************** //
    
    // Object Type
    
protected:
    
    ObjectType getType()
    {
        return mType;
    }

    // ************************************************************************************** //
    
    // IO Utilities
    
public:
    
    unsigned long getNumIns()
    {
        return mInputs.size();
    }
    
    unsigned long getNumOuts()
    {
        return mOutputs.size();
    }
    
    // ************************************************************************************** //
    
    // Input Modes
    
protected:
    
    // Call this from your constructor only (unsafe elsewhere)
    
    void inputMode(unsigned long idx, bool update, bool trigger, bool switchable)
    {
        mInputs[idx].mUpdate = update;
        mInputs[idx].mTrigger = trigger;
        mInputs[idx].mSwitchable = switchable;
    }
    
    // You should only call this from your update method (it is unsafe anywhere else)
    
    void updateTrigger(unsigned long idx, bool trigger)
    {
        // Update trigger only if switchable
        
        mInputs[idx].mTrigger = mInputs[idx].mSwitchable ? trigger : mInputs[idx].mTrigger;
    }
    
    // ************************************************************************************** //
    
    // Output Modes
    
protected:
    
    // Call this from your constructor only (unsafe elsewhere)
    
    void outputMode(unsigned long idx, OutputMode mode)
    {
        mOutputs[idx].mMode = mode;
    }

    // ************************************************************************************** //
    
    // Processing methods
 
private:
    
    // This returns true if the object requires notification from an audio thread
    
    bool requiresAudioNotification()
    {
        return handlesAudio() && mType != kOutput;
    }
    
    // Block updates for objects with audio IO
    
    void blockUpdate (double **ins, double **outs, unsigned long vecSize)
    {
        // Update block time and process the block
        
        mBlockTime += vecSize;
        blockProcess(ins, outs, vecSize);
        
        // If the object is not an output then notify
        
        if (requiresAudioNotification())
            notify(FALSE);
    }
    
    // Dependency notification
    
    inline void notify(bool releaseMemory)
    {
        if (releaseMemory)
            releaseOutputMemory();
    
        // If ready add to queue
        
        if (--mDependencyCount == 0)
            mQueue->add(this);
        
        // N.B. For multithreading re-entrancy needs to be avoided by increasing the dependency count before adding ot the queue (with match notification)
    }

    // Release output memory
    
    inline void releaseOutputMemory()
    {
        if (mOutputMemoryCount && (--mOutputMemoryCount == 0))
        {
            mAllocator->dealloc(mOutputs[0].mMemory);

            for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
            {
                outs->mMemory = NULL;
                outs->mCurrentSize = 0;
            }
        }
    }
    
    // Main code to control time flow (called when all input/output dependencies are ready)
    
    void dependenciesReady()
    {
        bool timeUpdated = TRUE;
        
        if (mType == kScheduler)
        {
            // It is disallowed to advance if the output already stretches beyond the current block time
            
            bool upToDate = mValidTime >= mBlockTime;
            
            SchedulerInfo scheduleInfo = schedule(mOutputDone, upToDate);
            
            // Check if time has been updated (limiting to positive advances only), and if so set output times
            
            scheduleInfo.mTimeAdvance = clipToPositive(scheduleInfo.mTimeAdvance);
            
            if (!scheduleInfo.mTimeAdvance || upToDate)
                timeUpdated = FALSE;
            else
            {
                mFrameTime = (scheduleInfo.mNewFrame || mOutputDone) ? mValidTime : mFrameTime;
                mValidTime += scheduleInfo.mTimeAdvance;
                mOutputDone = scheduleInfo.mOutputDone;
                upToDate = mValidTime >= mBlockTime;
            }
            
            // Find the new input time (the min valid time of all inputs)
            
            mInputTime = FL_Limits<FrameLib_TimeFormat>::largest();
            
            for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
                if (ins->mObject && ins->mObject->mValidTime < mInputTime)
                    mInputTime = ins->mObject->mValidTime;
            
            // If we are up to date with the block time (output and all inputs) add the dependency on the block update
            
            if (upToDate && mInputTime >= mBlockTime)
                mDependencyCount++;
        }
        else
        {
            bool trigger = FALSE;
            bool inputUpdate = FALSE;
            
            // Check for inputs at the current frame time that trigger or update
            
            for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
            {
                if ((ins->mObject && mValidTime == ins->mObject->mFrameTime))
                {
                    if (ins->mTrigger)
                        trigger = TRUE;
                    if (ins->mUpdate)
                        inputUpdate = TRUE;
                }
            }
            
            // Check for updates to be done before processing or scheduling
            
            if (inputUpdate)
                update();

            // Store previous valid till time to determine later if there has been any change
            
            FrameLib_TimeFormat prevValidTillTime = mValidTime;
            
            // Find the new valid till time (the min valid time of connected inputs that can trigger) and input time (the min valid time of all inputs)
            
            mInputTime = FL_Limits<FrameLib_TimeFormat>::largest();
            mValidTime = FL_Limits<FrameLib_TimeFormat>::largest();
            
            for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
            {
                if (ins->mObject && (ins->mTrigger || ins->mSwitchable) && ins->mObject->mValidTime < mValidTime)
                    mValidTime = ins->mObject->mValidTime;
                if (ins->mObject && ins->mObject->mValidTime < mInputTime)
                    mInputTime = ins->mObject->mValidTime;
            }

            // If triggered update the frame time, process and release the inputs if we only have one dependency
            
            if (trigger)
            {
                mFrameTime = prevValidTillTime;
                process();
                if (mInputDependencies.size() == 1)
                    (*mInputDependencies.begin())->releaseOutputMemory();
            }
            
            // Determine if time has updated
            
            timeUpdated = mValidTime != prevValidTillTime;

            // Check for completion
            
            if (timeUpdated)
            {
                mOutputDone = TRUE;
                
                for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
                {
                    if (ins->mObject && ((ins->mTrigger && !ins->mSwitchable) || (!ins->mObject->mOutputDone || ins->mSwitchable)) && (mValidTime == ins->mObject->mValidTime))
                    {
                        if (ins->mObject->mOutputDone)
                        {
                            mOutputDone = TRUE;
                            break;
                        }
                        mOutputDone = FALSE;
                    }
                }
            }
            
            // Check for block completion with objects requiring audio notification
            
            if (requiresAudioNotification() && mInputTime >= mBlockTime)
                mDependencyCount++;
        }
            
        // Update dependency for outputs
        
        mDependencyCount += (timeUpdated ? mOutputDependencies.size() : 0);
        
        // Notify input dependencies that can be released as they are up to date (releasing memory where relevant for objects with more than one input dependency)
        
        for (std::vector <FrameLib_DSP *>::iterator it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
        {
            if (mInputTime == (*it)->mValidTime)
            {
                mDependencyCount++;
                (*it)->notify((mType == kScheduler || mInputDependencies.size() != 1) && (*it)->mOutputDone);
            }
        }
        
        // If time has updated then notify output dependencies of calculation (updates to inputs)
        
        if (timeUpdated)
            for (std::vector <FrameLib_DSP *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
                (*it)->notify(FALSE);
    }
    
    // ************************************************************************************** //

    // Resets
    
public:

    void resetDependencyCount()
    {
        mOutputMemoryCount = 0;
        mDependencyCount = mInputDependencies.size() + ((requiresAudioNotification()) ? 1 : 0);
    }
    
    void reset()
    {
        // Note that the first sample will be at time == 1 so that we can start the frames *before* this with non-negative values
        
        mFrameTime = 0.0;
        mInputTime = 0.0;
        mValidTime = 1.0;
        mBlockTime = 1.0;
        mOutputDone = TRUE;
        
        resetDependencyCount();
    }
    
    // ************************************************************************************** //
    
    // Processing utilities
    
protected:
    
    bool isTrigger(unsigned long idx)
    {
        return mInputs[idx].mTrigger && (mInputs[idx].mObject->mFrameTime == mFrameTime);
    }
    
    FrameLib_TimeFormat getFrameTime()
    {
        return mFrameTime;
    }
    
    FrameLib_TimeFormat getValidTime()
    {
        return mValidTime;
    }
    
    FrameLib_TimeFormat getBlockTime()
    {
        return mBlockTime;
    }
    
    FrameLib_TimeFormat getInputFrameTime(unsigned long idx)
    {
        return mInputs[idx].mObject->mFrameTime;
    }
    
    FrameLib_TimeFormat getInputValidTillTime(unsigned long idx)
    {
        return mInputs[idx].mObject->mValidTime;
    }
    
    void requestOutputSize(unsigned long idx, size_t size)
    {
        mOutputs[idx].mRequestedSize = size;
    }
    
    bool allocateOutput()
    {
        size_t allocationSize = 0;
        
        for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
        {
            // Calculate allocation size, including necessary alignmnet padding and assuming success
            
            size_t unalignedSize = outs->mMode == kOutputNormal ? outs->mRequestedSize * sizeof(double) : outs->mRequestedSize + taggedOutputAlignment;
            size_t alignedSize = FrameLib_Memory::alignSize(unalignedSize);
            
            outs->mCurrentSize = outs->mRequestedSize;
            outs->mPointerOffset = allocationSize;
            
            allocationSize += alignedSize;
        }
        
        // Allocate memory
        
        if (mOutputs[0].mMemory)
            mAllocator->dealloc(mOutputs[0].mMemory);

        char *pointer = (char *) mAllocator->alloc(allocationSize);
        
        if (pointer)
        {
            // Store pointers and create tagged outputs
            
            for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
            {
                outs->mMemory = pointer + outs->mPointerOffset;
                
                if (outs->mMode == kOutputTagged)
                    new (outs->mMemory) FrameLib_Attributes::Serial(((BytePointer) outs->mMemory) + taggedOutputAlignment);
            }
            
            // Set dependency count
            
            mOutputMemoryCount = mOutputDependencies.size();
        
            return TRUE;
        }
        
        // Reset outputs on failure
        
        for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
        {
            outs->mMemory = NULL;
            outs->mCurrentSize = 0;
            mAllocator->dealloc(outs->mMemory);
            outs->mMemory = NULL;
        }
        
        mOutputMemoryCount = 0;
        
        return FALSE;
    }
    
    double *getOutput(unsigned long idx, size_t *size)
    {
        if (mOutputs[idx].mMode == kOutputNormal)
        {
            *size = mOutputs[idx].mCurrentSize;
            return (double *) mOutputs[idx].mMemory;
        }
        
        *size = 0;
        return NULL;
    }
    
    FrameLib_Attributes::Serial *getOutput(unsigned long idx)
    {
        if (mOutputs[idx].mMode == kOutputTagged)
            return (FrameLib_Attributes::Serial *) mOutputs[idx].mMemory;
        
        return NULL;
    }

    double *getInput(unsigned long idx, size_t *size)
    {
        if (mInputs[idx].mObject)
            return mInputs[idx].mObject->getOutput(mInputs[idx].mIndex, size);
        
        *size = 0;
        return NULL;
    }
    
    FrameLib_Attributes::Serial *getInput(unsigned long idx)
    {
        if (mOutputs[idx].mMode == kOutputTagged)
            return mInputs[idx].mObject->getOutput(mInputs[idx].mIndex);
        
        return NULL;
    }
    
    // ************************************************************************************** //
    
    // Audio handling reporting
    
    bool handlesAudio()
    {
        return (mType == kScheduler || getNumAudioIns() || getNumAudioOuts());
    }
    
    // ************************************************************************************** //
    
    // Customisable processing
    
private:
    
    // Override to handle audio at the block level (objects with block-based audio must overload this)
    
    virtual void blockProcess (double **ins, double **outs, unsigned long vecSize) {}
    
    // Override for updates prior to schedule / process (e.g. adjusting triggers)
    
    virtual void update() {}
    
    // Override for scheduling code (scheduler objects must override this)

    virtual SchedulerInfo schedule(bool newFrame, bool noOutput) = 0;
    
    // Override for main frame processing code (processor objects must override this)

    virtual void process() = 0;

    // ************************************************************************************** //

    // Connection mathods (private)
    
private:
    
    // Dependency updating
    
    std::vector <FrameLib_DSP *>::iterator removeInputDependency(FrameLib_DSP *object)
    {
        std::vector <FrameLib_DSP *>::iterator it;
        
        for (it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
            if (*it == object)
                return mInputDependencies.erase(it);
        
        return it;
    }
    
    std::vector <FrameLib_DSP *>::iterator removeOutputDependency(FrameLib_DSP *object)
    {
        std::vector <FrameLib_DSP *>::iterator it;
        
        for (it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            if (*it == object)
                return mOutputDependencies.erase(it);
        
        return it;
    }
    
    void addInputDependency(FrameLib_DSP *object)
    {
        for (std::vector <FrameLib_DSP *>::iterator it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
            if (*it == object)
                return;
        
        mInputDependencies.push_back(object);
    }
    
    void addOutputDependency(FrameLib_DSP *object)
    {
        for (std::vector <FrameLib_DSP *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            if (*it == object)
                return;
        
        mOutputDependencies.push_back(object);
    }
    
    // Removal of one connection to this object (before replacement / deletion)
    
    void removeConnection(unsigned long inIdx)
    {
        // Check that there is an object connected and that it is not connected to another input also
        
        if (!mInputs[inIdx].mObject)
            return;
        
        for (unsigned long i = 0; i < mInputs.size(); i++)
            if (mInputs[i].mObject == mInputs[inIdx].mObject && i != inIdx)
                return;
            
        // Update dependencies
        
        removeInputDependency(mInputs[inIdx].mObject);
        mInputs[inIdx].mObject->removeOutputDependency(this);
    }
    
    // Removal of all connections from one object to this object
    
    std::vector <FrameLib_DSP *>::iterator removeConnections(FrameLib_DSP *object)
    {
        // Set any inputs connected to the object to default values
        
        for (unsigned long i = 0; i < mInputs.size(); i++)
        {
            if (mInputs[i].mObject == object)
            {
                mInputs[i].mObject = NULL;
                mInputs[i].mIndex = 0;

            }
        }
        
        // Update dependencies

        removeInputDependency(object);
        return object->removeOutputDependency(this);
    }
   
    // ************************************************************************************** //
    
    // Connection methods (public)
    
public:
    
    // N.B. - No sanity checks here to maximise speed and help debugging (better for it to crash if a mistake is made)
    
    void deleteConnection(unsigned long inIdx)
    {
        // Update dependencies
        
        removeConnection(inIdx);
        
        // Set default values
        
        mInputs[inIdx].mObject = NULL;
        mInputs[inIdx].mIndex = 0;
        
        resetDependencyCount();
    }
    
    void addConnection(FrameLib_DSP *object, unsigned long outIdx, unsigned long inIdx)
    {
        // Update dependencies if the connection is now from a different object
        
        if (mInputs[inIdx].mObject != object)
        {
            removeConnection(inIdx);
            addInputDependency(object);
            object->addOutputDependency(this);
        }
        
        // Store data about connection
        
        mInputs[inIdx].mObject = object;
        mInputs[inIdx].mIndex = outIdx;
        
        resetDependencyCount();
    }
    
    void clearConnections()
    {
        // Remove input connections
        
        for (unsigned long i = 0; i < mInputs.size(); i++)
            deleteConnection(i);
        
        // Remove output connections
        
        for (std::vector <FrameLib_DSP *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); )
            it = (*it)->removeConnections(this);
        
        resetDependencyCount();
    }
    
    virtual bool isConnected(unsigned long inIdx)
    {
        return mInputs[inIdx].mObject != NULL;
    }
    
protected:
    
    FrameLib_DSP *getOutputObject(unsigned long outIdx)
    {
        return this;
    }
    
    // ************************************************************************************** //
    
    // Member variables
    
private:
    
    // DSP Queue
    
    DSPQueue *mQueue;
    FrameLib_DSP *mNext;
    
    // Memory Allocator
    
    FrameLib_Memory *mAllocator;
    
    // Object Type
    
    const ObjectType mType;
    
    // IO Info
    
    std::vector <Input> mInputs;
    std::vector <Output> mOutputs;
    
    std::vector <FrameLib_DSP *> mInputDependencies;
    std::vector <FrameLib_DSP *> mOutputDependencies;
    
    // Dependency Counts
    
    long mDependencyCount;
    long mOutputMemoryCount;
    
    // Frame and Block Timings
    
    FrameLib_TimeFormat mFrameTime;
    FrameLib_TimeFormat mValidTime;
    FrameLib_TimeFormat mInputTime;
    FrameLib_TimeFormat mBlockTime;
    
    bool mOutputDone;
};

// ************************************************************************************** //

// FrameLib_Processor - Simple class for process type objects

class FrameLib_Processor : public FrameLib_DSP
{
    
public:
    
    FrameLib_Processor(DSPQueue *queue, FrameLib_Memory *allocator, unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns = 0, unsigned long nAudioOuts = 0)
    : FrameLib_DSP(kProcessor, queue, allocator, nIns, nOuts, nAudioIns, nAudioOuts) {}
    
    FrameLib_Processor(FrameLib_Processor &object) : FrameLib_DSP(object) {}
    
protected:
    
    // Forces requirement to override
    
    virtual void process() = 0;
    
    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual SchedulerInfo schedule(bool newFrame, bool noOutput)
    {
        return SchedulerInfo();
    }
};

// ************************************************************************************** //

// FrameLib_Output - Simple class for output type objects

class FrameLib_Output : public FrameLib_DSP
{
    
public:

    FrameLib_Output(DSPQueue *queue, FrameLib_Memory *allocator, unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns = 0, unsigned long nAudioOuts = 0)
    : FrameLib_DSP(kOutput, queue, allocator, nIns, nOuts, nAudioIns, nAudioOuts) {}
    
    FrameLib_Output(FrameLib_Output &object) : FrameLib_DSP(object) {}

protected:

    // Forces requirement to override
    
    virtual void process() = 0;
    
    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual SchedulerInfo schedule(bool newFrame, bool noOutput)
    {
        return SchedulerInfo();
    }
};

// ************************************************************************************** //

// FrameLib_Scheduler - Simple class for scheduler type objects

class FrameLib_Scheduler : public FrameLib_DSP
{

public:
    
    FrameLib_Scheduler(DSPQueue *queue, FrameLib_Memory *allocator, unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns = 0, unsigned long nAudioOuts = 0)
    : FrameLib_DSP(kScheduler, queue, allocator, nIns, nOuts, nAudioIns, nAudioOuts) {}
    
    FrameLib_Scheduler(FrameLib_Scheduler &object) : FrameLib_DSP(object) {}

protected:

    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual void process(){}
    
    // Forces requirement to override
    
    virtual SchedulerInfo schedule(bool newFrame, bool noOutput) = 0;
};

#endif

