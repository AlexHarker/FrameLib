
#ifndef FRAMELIB_DSP_H
#define FRAMELIB_DSP_H

#include "FrameLib_Types.h"
#include "FrameLib_Block.h"
#include <limits>
#include <vector>

// FIX - Processor and Output look identical - could it be that only one is needed now?

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
        
        DSPQueue(FrameLib_Local_Allocator *allocator) : mAllocator(allocator), mTop(NULL), mTail(NULL) {}
        
        // FIX - check assumption that an object can only be in the queue once at a time!
        
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
        
        FrameLib_Local_Allocator *getAllocator()
        {
            return mAllocator;
        }
        
    private:

        FrameLib_Local_Allocator *mAllocator;
        
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

    // Constructors
    
public:
    
    FrameLib_DSP (ObjectType type, DSPQueue *queue, unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns, unsigned long nAudioOuts)
    : FrameLib_Block(nAudioIns, nAudioOuts), mAllocator(queue->getAllocator()), mQueue(queue), mNext(NULL), mType(type)
    {
        // Set IO
        
        setIO(nIns, nOuts, nAudioIns, nAudioOuts);
        
        setSamplingRate(0.0);
    }
    
    // Destructor
    
    ~FrameLib_DSP ()
    {
        // Clear connections before deleting
        
        clearConnections();
    }
    
    // ************************************************************************************** //

    // Sampling Rate

public:
    
    void setSamplingRate(double samplingRate)
    {
        mSamplingRate = samplingRate > 0 ? samplingRate : 44100.0;
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
    
protected:
    
    // Call this in derived class constructors only if the IO size is not fixed
   
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns = 0, unsigned long nAudioOuts = 0)
    {
        // Free output memory
        
        freeOutputMemory();

        // Resize inputs and outputs
    
        mInputs.resize((mType != kScheduler || nIns > 0) ? nIns : 1);
        mOutputs.resize(nOuts);
        
        FrameLib_Block::setIO(nAudioIns, nAudioOuts);

        // Check if the object handles audio
        
        updateHandlesAudio();

        // Reset for audio
        
        reset();
    }
    
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
        return mRequiresAudioNotification;
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
        
        // N.B. For multithreading re-entrancy needs to be avoided by increasing the dependency count before adding to the queue (with matching notification)
    }

    // Release output memory
    
    inline void releaseOutputMemory()
    {
        if (mOutputMemoryCount && (--mOutputMemoryCount == 0))
            freeOutputMemory();
    }
    
    // Main code to control time flow (called when all input/output dependencies are ready)
    
    void dependenciesReady()
    {        
        bool timeUpdated = FALSE;
        
        if (mType == kScheduler)
        {
            // It is disallowed to advance if the output already stretches beyond the current block time
            
            bool upToDate = mValidTime >= mBlockTime;
            
            SchedulerInfo scheduleInfo = schedule(mOutputDone, upToDate);
            
            // Check if time has been updated (limiting to positive advances only), and if so set output times
            
            scheduleInfo.mTimeAdvance = clipToPositive(scheduleInfo.mTimeAdvance);
            
            if (scheduleInfo.mTimeAdvance && !upToDate)
            {
                mFrameTime = (scheduleInfo.mNewFrame || mOutputDone) ? mValidTime : mFrameTime;
                mValidTime += scheduleInfo.mTimeAdvance;
                mOutputDone = scheduleInfo.mOutputDone;
                upToDate = mValidTime >= mBlockTime;
                timeUpdated = TRUE;
            }
            
            // Find the new input time (the min valid time of all inputs)
            // FIX - move above (to make scheduling easier?) - although currently inputtime is inaccessible - make function?
            
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

            // Check for inputs at the current frame time that update
            
            for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
            {
                if (ins->mUpdate && ins->mObject && mValidTime == ins->mObject->mFrameTime)
                {
                    update();
                    break;
                }
            }
            
            // Check for inputs at the current frame time that trigger (after any update)
            
            for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
            {
                if (ins->mTrigger && ins->mObject && mValidTime == ins->mObject->mFrameTime)
                {
                    trigger = TRUE;
                    break;
                }
            }

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
            // FIX - is it possible in a process object for time not to update???
            
            timeUpdated = mValidTime != prevValidTillTime;

            // Check for completion
            
            if (timeUpdated)
            {
                mOutputDone = TRUE;
                
                for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
                {
                    // FIX - I don't get the second of these tests
                    
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
            
            // Check for block completion for objects requiring audio notification
            
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
    
private:
    
    inline void freeOutputMemory()
    {
        if (getNumOuts())
        {
            mAllocator->dealloc(mOutputs[0].mMemory);
            mOutputs[0].mMemory = NULL;
        }
    }
    
    void resetDependencyCount()
    {
        mOutputMemoryCount = 0;
        mDependencyCount = mInputDependencies.size() + ((requiresAudioNotification()) ? 1 : 0);
        
        freeOutputMemory();
    }
    
public:

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
    
    // FIX - you can't just pretend these will all be fine to access - these times need safety checks...!!!!!!!!!!!
    
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
    
    bool allocateOutputs()
    {
        size_t allocationSize = 0;
        
        for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
        {
            // Calculate allocation size, including necessary alignment padding and assuming success
            
            // FIX - check serial alignment safety - move this into the class to be on the safe side??
            
            size_t unalignedSize = outs->mMode == kOutputNormal ? outs->mRequestedSize * sizeof(double) : outs->mRequestedSize + sizeof(FrameLib_Attributes::Serial) + taggedOutputAlignment;
            size_t alignedSize = FrameLib_Local_Allocator::alignSize(unalignedSize);
            
            outs->mCurrentSize = outs->mRequestedSize;
            outs->mPointerOffset = allocationSize;
            
            allocationSize += alignedSize;
        }
        
        // Allocate memory
        
        if (mOutputs[0].mMemory)
            mAllocator->dealloc(mOutputs[0].mMemory);

        char *pointer = !allocationSize ? NULL : (char *) mAllocator->alloc(allocationSize);
        
        if (pointer)
        {
            // Store pointers and create tagged outputs
            
            for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
            {
                outs->mMemory = pointer + outs->mPointerOffset;
                
                if (outs->mMode == kOutputTagged)
                    new (outs->mMemory) FrameLib_Attributes::Serial(((BytePointer) outs->mMemory) + sizeof(FrameLib_Attributes::Serial), outs->mCurrentSize);
            }
            
            // Set dependency count
            
            mOutputMemoryCount = mOutputDependencies.size();
        
            return TRUE;
        }
        
        // Reset outputs on failure of zero size
        
        for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
        {
            outs->mMemory = NULL;
            outs->mCurrentSize = 0;
        }
        
        mOutputMemoryCount = 0;
        
        return FALSE;
    }
    
    double *getOutput(unsigned long idx, size_t *size)
    {
        if (mOutputs[0].mMemory && mOutputs[idx].mMode == kOutputNormal)
        {
            *size = mOutputs[idx].mCurrentSize;
            return (double *) mOutputs[idx].mMemory;
        }
        
        *size = 0;
        return NULL;
    }
    
    FrameLib_Attributes::Serial *getOutput(unsigned long idx)
    {
        if (mOutputs[0].mMemory && mOutputs[idx].mMode == kOutputTagged)
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
        if (mInputs[idx].mObject)
            return mInputs[idx].mObject->getOutput(mInputs[idx].mIndex);
        
        return NULL;
    }
    
    // ************************************************************************************** //
    
    // Audio handling reporting
    
    void updateHandlesAudio()
    {
        mHandlesAudio = (mType == kScheduler || getNumAudioIns() || getNumAudioOuts());
        mRequiresAudioNotification = handlesAudio() && mType != kOutput;
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
   
protected:
    
    // Sampling Rate
    
    double mSamplingRate;
    
    // Memory Allocator
    
    FrameLib_Local_Allocator *mAllocator;
    
private:
    
    // DSP Queue
    
    DSPQueue *mQueue;
    FrameLib_DSP *mNext;
    
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
    
    // Cached Flag for Audio Notification
    
    bool mRequiresAudioNotification;
};

// ************************************************************************************** //

// FrameLib_Processor - Simple class for process type objects

class FrameLib_Processor : public FrameLib_DSP
{
    
public:
    
    FrameLib_Processor(DSPQueue *queue, unsigned long nIns = 0, unsigned long nOuts = 0, unsigned long nAudioIns = 0, unsigned long nAudioOuts = 0)
    : FrameLib_DSP(kProcessor, queue, nIns, nOuts, nAudioIns, nAudioOuts) {}
        
protected:
    
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

    FrameLib_Output(DSPQueue *queue, unsigned long nIns = 0, unsigned long nOuts = 0, unsigned long nAudioIns = 0, unsigned long nAudioOuts = 0)
    : FrameLib_DSP(kOutput, queue, nIns, nOuts, nAudioIns, nAudioOuts) {}
    
protected:

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
    
    FrameLib_Scheduler(DSPQueue *queue, unsigned long nIns = 0, unsigned long nOuts = 0, unsigned long nAudioIns = 0, unsigned long nAudioOuts = 0)
    : FrameLib_DSP(kScheduler, queue, nIns, nOuts, nAudioIns, nAudioOuts) {}
    
protected:

    // This prevents the user from needing to implement this method - doing so will do nothing
    
    virtual void process(){}
};

#endif

