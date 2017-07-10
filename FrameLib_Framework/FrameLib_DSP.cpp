

#include "FrameLib_DSP.h"
#include <algorithm>

// Constructor / Destructor

FrameLib_DSP::FrameLib_DSP(ObjectType type, FrameLib_Context context, unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans)
: mAllocator(context), mQueue(context), mNext(NULL), mType(type), mInUpdate(false)
{
    // Set IO
    
    setIO(nIns, nOuts, nAudioChans);
    
    setSamplingRate(0.0);
}

// Destructor

FrameLib_DSP::~FrameLib_DSP()
{
    // Clear connections before deleting
    
    clearConnections();
    
    // Delete Fixed Inputs
    
    for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
        mAllocator->dealloc(ins->mFixedInput);
}

// Set Fixed Input

void FrameLib_DSP::setFixedInput(unsigned long idx, double *input, unsigned long size)
{
    // This is not threadsafe with the processing thread and should not be called concurrently...
    
    mInputs[idx].mSize = 0;
    
    if (mInputs[idx].mFixedInput)
        mAllocator->dealloc(mInputs[idx].mFixedInput);
    
    mInputs[idx].mFixedInput = (double *) mAllocator->alloc(size * sizeof(double));
    
    if (mInputs[idx].mFixedInput)
    {
        std::copy(input, input + size, mInputs[idx].mFixedInput);
        mInputs[idx].mSize = size;
    }
}


// Audio Processing

// Block updates for objects with audio IO

void FrameLib_DSP::blockUpdate(double **ins, double **outs, unsigned long vecSize)
{
    // Update block time and process the block
    
    mBlockEndTime += vecSize;
    blockProcess(ins, outs, vecSize);
    
    // If the object is handling audio updates (but is not an output object) then notify
    
    if (mValidTime < mBlockEndTime && requiresAudioNotification())
        dependencyNotify(false);
    
    mBlockStartTime = mBlockEndTime;
}

// Reset

void FrameLib_DSP::reset()
{
    // Note that the first sample will be at time == 1 so that we can start the frames *before* this with non-negative values
    
    mFrameTime = 0.0;
    mInputTime = 0.0;
    mValidTime = 1.0;
    mBlockStartTime = 1.0;
    mBlockEndTime = 1.0;
    mOutputDone = true;
    
    resetDependencyCount();
}

// Connection Methods

void FrameLib_DSP::deleteConnection(unsigned long inIdx)
{
    removeConnection(inIdx);
    clearConnection(inIdx);
    resetDependencyCount();
}

void FrameLib_DSP::addConnection(FrameLib_DSP *object, unsigned long outIdx, unsigned long inIdx)
{
    // Update dependencies if the connection is now from a different object
    
    if (mInputs[inIdx].mObject != object)
    {
        removeConnection(inIdx);
        addInputDependency(object);
        object->addOutputDependency(this);
    }
    
    // Store data about connection and reset the dependency count
    
    mInputs[inIdx].SetInput(object, outIdx);
    resetDependencyCount();
}

void FrameLib_DSP::clearConnections()
{
    // Remove input connections
    
    for (unsigned long i = 0; i < mInputs.size(); i++)
        deleteConnection(i);
    
    // Remove output connections
    
    for (std::vector <FrameLib_DSP *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); )
        it = (*it)->disconnect(this);
    
    resetDependencyCount();
}

bool FrameLib_DSP::isConnected(unsigned long inIdx)
{
    return mInputs[inIdx].mObject != NULL;
}

// Setup and IO Modes

void FrameLib_DSP::setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans)
{
    // Free output memory
    
    freeOutputMemory();
    
    // Resize inputs and outputs
    
    mInputs.resize((mType != kScheduler || nIns > 0) ? nIns : 1);
    mOutputs.resize(nOuts);
    
    FrameLib_Block::setIO(FrameLib_Block::IOCount(mInputs.size(), mOutputs.size(), getType() != kOutput ? nAudioChans : 0, getType() == kOutput ? nAudioChans : 0));
    
    // Reset for audio
    
    reset();
}

// Call this from your constructor only (unsafe elsewhere)

void FrameLib_DSP::inputMode(unsigned long idx, bool update, bool trigger, bool switchable)
{
    mInputs[idx].mUpdate = update;
    mInputs[idx].mTrigger = trigger;
    mInputs[idx].mSwitchable = switchable;
}

// Call this from your constructor only (unsafe elsewhere)

void FrameLib_DSP::outputMode(unsigned long idx, OutputMode mode)
{
    mOutputs[idx].mMode = mode;
}

// You should only call this from your update method

void FrameLib_DSP::updateTrigger(unsigned long idx, bool trigger)
{
    // Update trigger only if switchable and we are in the update method
    
    mInputs[idx].mTrigger = mInputs[idx].mSwitchable && mInUpdate ? trigger : mInputs[idx].mTrigger;
}

// Output Allocation

bool FrameLib_DSP::allocateOutputs()
{
    size_t allocationSize = 0;
    
    for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
    {
        // Calculate allocation size, including necessary alignment padding and assuming success
        
        size_t unalignedSize = outs->mMode == kOutputNormal ? outs->mRequestedSize * sizeof(double) : SerialBase::inPlaceSize(outs->mRequestedSize);
        size_t alignedSize = FrameLib_LocalAllocator::alignSize(unalignedSize);
        
        outs->mCurrentSize = outs->mRequestedSize;
        outs->mPointerOffset = allocationSize;
        
        allocationSize += alignedSize;
    }
    
    // Free then allocate memory
    
    freeOutputMemory();
    
    BytePointer pointer = (BytePointer) mAllocator->alloc(allocationSize);
    
    if (pointer)
    {
        // Store pointers and create tagged outputs
        
        for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
        {
            outs->mMemory = pointer + outs->mPointerOffset;
            
            if (outs->mMode == kOutputTagged)
                SerialBase::newInPlace(outs->mMemory, outs->mCurrentSize);
        }
        
        // Set dependency count
        
        mOutputMemoryCount = mOutputDependencies.size();
        
        return true;
    }
    
    // Reset outputs on failure of zero size
    
    for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
    {
        outs->mMemory = NULL;
        outs->mCurrentSize = 0;
    }
    
    mOutputMemoryCount = 0;
    
    return false;
}

// Get Inputs and Outputs

double *FrameLib_DSP::getInput(unsigned long idx, size_t *size)
{
    if (mInputs[idx].mObject)
        return mInputs[idx].mObject->getOutput(mInputs[idx].mIndex, size);
    
    *size = mInputs[idx].mSize;
    return mInputs[idx].mFixedInput;
}

FrameLib_Parameters::SerialBase *FrameLib_DSP::getInput(unsigned long idx)
{
    if (mInputs[idx].mObject)
        return mInputs[idx].mObject->getOutput(mInputs[idx].mIndex);
    
    return NULL;
}

double *FrameLib_DSP::getOutput(unsigned long idx, size_t *size)
{
    if (mOutputs[0].mMemory && mOutputs[idx].mMode == kOutputNormal)
    {
        *size = mOutputs[idx].mCurrentSize;
        return (double *) mOutputs[idx].mMemory;
    }
    
    *size = 0;
    return NULL;
}

FrameLib_Parameters::SerialBase *FrameLib_DSP::getOutput(unsigned long idx)
{
    if (mOutputs[0].mMemory && mOutputs[idx].mMode == kOutputTagged)
        return (SerialBase *) mOutputs[idx].mMemory;
    
    return NULL;
}

// Dependency Notification

inline void FrameLib_DSP::dependencyNotify(bool releaseMemory)
{
    if (releaseMemory)
        releaseOutputMemory();
    
    // If ready add to queue
    
    if (--mDependencyCount == 0)
        mQueue->add(this);
    
    // N.B. For multithreading re-entrancy needs to be avoided by increasing the dependency count before adding to the queue (with matching notification)
}

// Main code to control time flow (called when all input/output dependencies are ready)

void FrameLib_DSP::dependenciesReady()
{
    bool timeUpdated = false;
    
    if (mType == kScheduler)
    {
        // Find the input time (the min valid time of all inputs)
        
        mInputTime = FL_Limits<FrameLib_TimeFormat>::largest();
        
        for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
            if (ins->mObject && ins->mObject->mValidTime < mInputTime)
                mInputTime = ins->mObject->mValidTime;
        
        // It is disallowed to advance if the output already stretches beyond the current block time
        
        bool upToDate = mValidTime >= mBlockEndTime;
        
        SchedulerInfo scheduleInfo = schedule(mOutputDone, upToDate);
        
        // Check if time has been updated (limiting to positive advances only), and if so set output times
        
        scheduleInfo.mTimeAdvance = clipToPositive(scheduleInfo.mTimeAdvance);
        
        if (scheduleInfo.mTimeAdvance && !upToDate)
        {
            mFrameTime = (scheduleInfo.mNewFrame || mOutputDone) ? mValidTime : mFrameTime;
            mValidTime += scheduleInfo.mTimeAdvance;
            mOutputDone = scheduleInfo.mOutputDone;
            upToDate = mValidTime >= mBlockEndTime;
            timeUpdated = true;
        }
        
        // If we are up to date with the block time (output and all inputs) add the dependency on the block update
        
        if (upToDate && mInputTime >= mBlockEndTime)
            mDependencyCount++;
    }
    else
    {
        bool trigger = false;
        
        // Check for inputs at the current frame time that update
        
        for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
        {
            if (ins->mUpdate && ins->mObject && mValidTime == ins->mObject->mFrameTime)
            {
                mInUpdate = true;
                update();
                mInUpdate = false;
                break;
            }
        }
        
        // Check for inputs at the current frame time that trigger (after any update)
        
        for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
        {
            if (ins->mTrigger && ins->mObject && mValidTime == ins->mObject->mFrameTime)
            {
                trigger = true;
                break;
            }
        }
        
        // Store previous valid till time to determine later if there has been any change
        
        FrameLib_TimeFormat prevValidTillTime = mValidTime;
        
        // Find the valid till time (the min valid time of connected inputs that can trigger) and input time (the min valid time of all inputs)
        
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
        
        // It shouldn't be it possible in a process object for time not to update but just in case...
        
        timeUpdated = mValidTime != prevValidTillTime;
        
        // Check for completion
        
        if (timeUpdated)
        {
            mOutputDone = true;
            
            for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
            {
                if (ins->mObject && ((ins->mTrigger && !ins->mSwitchable) || (!ins->mObject->mOutputDone || ins->mSwitchable)) && (mValidTime == ins->mObject->mValidTime))
                {
                    if (ins->mObject->mOutputDone)
                    {
                        mOutputDone = true;
                        break;
                    }
                    mOutputDone = false;
                }
            }
        }
        
        // Check for block completion for objects requiring audio notification
        
        if (requiresAudioNotification() && mValidTime >= mBlockEndTime)
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
            (*it)->dependencyNotify((mType == kScheduler || mInputDependencies.size() != 1) && (*it)->mOutputDone);
        }
    }
    
    // If time has updated then notify output dependencies of calculation (updates to inputs)
    
    if (timeUpdated)
        for (std::vector <FrameLib_DSP *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            (*it)->dependencyNotify(false);
}

void FrameLib_DSP::resetDependencyCount()
{
    mOutputMemoryCount = 0;
    mDependencyCount = mInputDependencies.size() + ((requiresAudioNotification()) ? 1 : 0);
    
    freeOutputMemory();
}

// Manage Output Memory

inline void FrameLib_DSP::freeOutputMemory()
{
    if (getNumOuts() && mOutputs[0].mMemory)
    {
        // FIX - revise this as there is nothing for the destructor to do (except branch on a call to delete[])
        
        // Call the destructor for any serial outputs
        
        for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
            if (outs->mMode == kOutputTagged)
                ((SerialBase *)outs->mMemory)->SerialBase::~SerialBase();

        // Then deallocate
        
        mAllocator->dealloc(mOutputs[0].mMemory);
   
        mOutputs[0].mMemory = NULL;
    }
}

inline void FrameLib_DSP::releaseOutputMemory()
{
    if (--mOutputMemoryCount == 0)
        freeOutputMemory();
}

// Connection Methods (private)

// Dependency Updating

std::vector <FrameLib_DSP *>::iterator FrameLib_DSP::removeInputDependency(FrameLib_DSP *object)
{
    std::vector <FrameLib_DSP *>::iterator it;
    
    for (it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
        if (*it == object)
            return mInputDependencies.erase(it);
    
    return it;
}

std::vector <FrameLib_DSP *>::iterator FrameLib_DSP::removeOutputDependency(FrameLib_DSP *object)
{
    std::vector <FrameLib_DSP *>::iterator it;
    
    for (it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
        if (*it == object)
            return mOutputDependencies.erase(it);
    
    return it;
}

void FrameLib_DSP::addInputDependency(FrameLib_DSP *object)
{
    for (std::vector <FrameLib_DSP *>::iterator it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
        if (*it == object)
            return;
    
    mInputDependencies.push_back(object);
}

void FrameLib_DSP::addOutputDependency(FrameLib_DSP *object)
{
    for (std::vector <FrameLib_DSP *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
        if (*it == object)
            return;
    
    mOutputDependencies.push_back(object);
}

// Remove connection and set to defaults

void FrameLib_DSP::clearConnection(unsigned long inIdx)
{
    removeConnection(inIdx);
    mInputs[inIdx].SetInput();
}

// Removal of one connection to this object (before replacement / deletion)

void FrameLib_DSP::removeConnection(unsigned long inIdx)
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

std::vector <FrameLib_DSP *>::iterator FrameLib_DSP::disconnect(FrameLib_DSP *object)
{
    // Set any inputs connected to the object to default values
    
    for (unsigned long i = 0; i < mInputs.size(); i++)
        if (mInputs[i].mObject == object)
            mInputs[i].SetInput();

    // Update dependencies
    
    removeInputDependency(object);
    return object->removeOutputDependency(this);
}
