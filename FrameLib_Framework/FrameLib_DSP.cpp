

#include "FrameLib_DSP.h"

// Constructor / Destructor

FrameLib_DSP::FrameLib_DSP (ObjectType type, FrameLib_Context context, unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns, unsigned long nAudioOuts)
: FrameLib_Block(nAudioIns, nAudioOuts), mContext(context), mAllocator(context.getAllocator()), mQueue(context.getDSPQueue()), mNext(NULL), mType(type)
{
    // Set IO
    
    setIO(nIns, nOuts, nAudioIns, nAudioOuts);
    
    setSamplingRate(0.0);
}

// Destructor

FrameLib_DSP::~FrameLib_DSP ()
{
    // Clear connections before deleting
    
    clearConnections();
    
    // Delete Fixed Inputs
    
    for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
        mAllocator->dealloc(ins->mFixedInput);
    
    mContext.releaseDSPQueue();
    mContext.releaseAllocator();
}

// ************************************************************************************** //

// Set IO Size

void FrameLib_DSP::setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns, unsigned long nAudioOuts)
{
    // Free output memory
    
    freeOutputMemory();
    
    // Resize inputs and outputs
    
    mInputs.resize((mType != kScheduler || nIns > 0) ? nIns : 1);
    mOutputs.resize(nOuts);
    
    FrameLib_Block::setIO(nAudioIns, nAudioOuts);
    
    // Check if the object handles audio
    
    //updateHandlesAudio();
    
    // Reset for audio
    
    reset();
}

// ************************************************************************************** //

// Input / Output Modes

// Call this from your constructor only (unsafe elsewhere)

void FrameLib_DSP::inputMode(unsigned long idx, bool update, bool trigger, bool switchable)
{
    mInputs[idx].mUpdate = update;
    mInputs[idx].mTrigger = trigger;
    mInputs[idx].mSwitchable = switchable;
}

// You should only call this from your update method (it is unsafe anywhere else)

void FrameLib_DSP::updateTrigger(unsigned long idx, bool trigger)
{
    // Update trigger only if switchable
    
    mInputs[idx].mTrigger = mInputs[idx].mSwitchable ? trigger : mInputs[idx].mTrigger;
}

// Call this from your constructor only (unsafe elsewhere)

void FrameLib_DSP::outputMode(unsigned long idx, OutputMode mode)
{
    mOutputs[idx].mMode = mode;
}

// ************************************************************************************** //

// Processing methods

// Block updates for objects with audio IO
    
void FrameLib_DSP::blockUpdate(double **ins, double **outs, unsigned long vecSize)
{
    // Update block time and process the block
    
    mBlockEndTime += vecSize;
    blockProcessPre(ins, outs, vecSize);
    
    // If the object is not an output then notify
    
    if (mValidTime < mBlockEndTime && requiresAudioNotification())
        notify(false);
    
    blockProcessPost(ins, outs, vecSize);
    mBlockStartTime = mBlockEndTime;
}

// Dependency notification

inline void FrameLib_DSP::notify(bool releaseMemory)
{
    if (releaseMemory)
        releaseOutputMemory();
    
    // If ready add to queue
    
    if (--mDependencyCount == 0)
        mQueue->add(this);
    
    // N.B. For multithreading re-entrancy needs to be avoided by increasing the dependency count before adding to the queue (with matching notification)
}

// Release output memory

inline void FrameLib_DSP::releaseOutputMemory()
{
    if (mOutputMemoryCount && (--mOutputMemoryCount == 0))
        freeOutputMemory();
}

// Main code to control time flow (called when all input/output dependencies are ready)

void FrameLib_DSP::dependenciesReady()
{
    bool timeUpdated = false;
    
    if (mType == kScheduler)
    {
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
        
        // Find the new input time (the min valid time of all inputs)
        // FIX - move above (to make scheduling easier?) - although currently inputtime is inaccessible - make function?
        
        mInputTime = FL_Limits<FrameLib_TimeFormat>::largest();
        
        for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
            if (ins->mObject && ins->mObject->mValidTime < mInputTime)
                mInputTime = ins->mObject->mValidTime;
        
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
                update();
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
            (*it)->notify((mType == kScheduler || mInputDependencies.size() != 1) && (*it)->mOutputDone);
        }
    }
    
    // If time has updated then notify output dependencies of calculation (updates to inputs)
    
    if (timeUpdated)
        for (std::vector <FrameLib_DSP *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            (*it)->notify(false);
}

// ************************************************************************************** //

// Resets

inline void FrameLib_DSP::freeOutputMemory()
{
    if (getNumOuts())
    {
        mAllocator->dealloc(mOutputs[0].mMemory);
        mOutputs[0].mMemory = NULL;
    }
}

void FrameLib_DSP::resetDependencyCount()
{
    mOutputMemoryCount = 0;
    mDependencyCount = mInputDependencies.size() + ((requiresAudioNotification()) ? 1 : 0);
    
    freeOutputMemory();
}

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

void FrameLib_DSP::setFixedInput(unsigned long idx, double *input, unsigned long size)
{
    // This is not threadsafe with the processing thread and should not be called concurrently...
    
    mInputs[idx].mSize = 0;
    
    if (mInputs[idx].mFixedInput)
        mAllocator->dealloc(mInputs[idx].mFixedInput);
    
    mInputs[idx].mFixedInput = (double *) mAllocator->alloc(size * sizeof(double));
    
    if (mInputs[idx].mFixedInput)
    {
        for (unsigned long i = 0; i < size; i++)
            mInputs[idx].mFixedInput[i] = input[i];
        
        mInputs[idx].mSize = size;
    }
}

// ************************************************************************************** //

// Processing utilities

bool FrameLib_DSP::allocateOutputs()
{
    size_t allocationSize = 0;
    
    for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
    {
        // Calculate allocation size, including necessary alignment padding and assuming success
        
        // FIX - check serial alignment safety - move this into the class to be on the safe side??
        
        size_t unalignedSize = outs->mMode == kOutputNormal ? outs->mRequestedSize * sizeof(double) : outs->mRequestedSize + sizeof(FrameLib_Attributes::Serial) + taggedOutputAlignment;
        size_t alignedSize = FrameLib_LocalAllocator::alignSize(unalignedSize);
        
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
            
            // FIX - where does the destructor get called (I know it shouldn't do anything)
            
            if (outs->mMode == kOutputTagged)
                new (outs->mMemory) FrameLib_Attributes::Serial(((BytePointer) outs->mMemory) + sizeof(FrameLib_Attributes::Serial), outs->mCurrentSize);
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

FrameLib_Attributes::Serial *FrameLib_DSP::getOutput(unsigned long idx)
{
    if (mOutputs[0].mMemory && mOutputs[idx].mMode == kOutputTagged)
        return (FrameLib_Attributes::Serial *) mOutputs[idx].mMemory;
    
    return NULL;
}

double *FrameLib_DSP::getInput(unsigned long idx, size_t *size)
{
    if (mInputs[idx].mObject)
        return mInputs[idx].mObject->getOutput(mInputs[idx].mIndex, size);
    
    *size = mInputs[idx].mSize;
    return mInputs[idx].mFixedInput;
}

FrameLib_Attributes::Serial *FrameLib_DSP::getInput(unsigned long idx)
{
    if (mInputs[idx].mObject)
        return mInputs[idx].mObject->getOutput(mInputs[idx].mIndex);
    
    return NULL;
}

// ************************************************************************************** //

// Connection mathods (private)

// Dependency updating

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

std::vector <FrameLib_DSP *>::iterator FrameLib_DSP::removeConnections(FrameLib_DSP *object)
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

void FrameLib_DSP::deleteConnection(unsigned long inIdx)
{
    // Update dependencies
    
    removeConnection(inIdx);
    
    // Set default values
    
    mInputs[inIdx].mObject = NULL;
    mInputs[inIdx].mIndex = 0;
    
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
    
    // Store data about connection
    
    mInputs[inIdx].mObject = object;
    mInputs[inIdx].mIndex = outIdx;
    
    resetDependencyCount();
}

void FrameLib_DSP::clearConnections()
{
    // Remove input connections
    
    for (unsigned long i = 0; i < mInputs.size(); i++)
        deleteConnection(i);
    
    // Remove output connections
    
    for (std::vector <FrameLib_DSP *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); )
        it = (*it)->removeConnections(this);
    
    resetDependencyCount();
}

bool FrameLib_DSP::isConnected(unsigned long inIdx)
{
    return mInputs[inIdx].mObject != NULL;
}



