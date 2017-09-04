
#include "FrameLib_DSP.h"

// Constructor / Destructor

FrameLib_DSP::FrameLib_DSP(ObjectType type, FrameLib_Context context, FrameLib_Parameters::Info *info, unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans)
: FrameLib_Block(type), mSamplingRate(44100.0), mMaxBlockSize(4096), mAllocator(context), mParameters(info), mQueue(context), mNext(NULL), mInUpdate(false)
{
    // Set IO
    
    setIO(nIns, nOuts, nAudioChans);
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
        copyVector(mInputs[idx].mFixedInput, input, size);
        mInputs[idx].mSize = size;
    }
}

// Audio Processing

// Block updates for objects with audio IO

void FrameLib_DSP::blockUpdate(double **ins, double **outs, unsigned long blockSize)
{
    // Update block time and process the block
    
    mBlockStartTime = mBlockEndTime;
    mBlockEndTime += blockSize;
    blockProcess(ins, outs, blockSize);
    
    // If the object is handling audio updates (but is not an output object) then notify
    
    if (requiresAudioNotification())
        dependencyNotify(false, true);
}

// Reset

void FrameLib_DSP::reset(double samplingRate, unsigned long maxBlockSize)
{
    // Store sample rate / max block size and call object specific reset
    
    mSamplingRate = samplingRate > 0 ? samplingRate : 44100.0;
    mMaxBlockSize = maxBlockSize;
    
    objectReset();
    
    // Note that the first sample will be at time == 1 so that we can start the frames *before* this with non-negative values
    
    mFrameTime = 0.0;
    mInputTime = 1.0;
    mValidTime = 1.0;
    mBlockStartTime = 1.0;
    mBlockEndTime = 1.0;
    
    mOutputDone = false;
    
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
    
    mInputs[inIdx].setInput(object, outIdx);
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
    
    // Call the base class to store new sizes
    
    FrameLib_Block::setIO(nIns, nOuts, nAudioChans);
    
    // Resize inputs and outputs
    
    mInputs.resize(getNumIns());
    mOutputs.resize(getNumOuts());
    
    // Reset for audio
    
    FrameLib_DSP::reset(mSamplingRate, mMaxBlockSize);
}

// Call this from your constructor only (unsafe elsewhere)

void FrameLib_DSP::inputMode(unsigned long idx, bool update, bool trigger, bool switchable, FrameType type)
{
    mInputs[idx].mUpdate = update;
    mInputs[idx].mTrigger = trigger;
    mInputs[idx].mSwitchable = switchable;
    mInputs[idx].mType = type;
}

// Call this from your constructor only (unsafe elsewhere)

void FrameLib_DSP::setParameterInput(unsigned long idx)
{
    inputMode(idx, true, false, false, kFrameTagged);
    mInputs[idx].mParameters = true;
}

// Call this from your constructor only (unsafe elsewhere)

void FrameLib_DSP::addParameterInput()
{
    unsigned long nIns = getNumIns();
    unsigned long nOuts = getNumOuts();
    unsigned long nAudioChans = getNumAudioChans();
    
    setIO(nIns, nOuts + 1, nAudioChans);
    setParameterInput(nOuts);
}

// Call this from your constructor only (unsafe elsewhere)

void FrameLib_DSP::outputMode(unsigned long idx, FrameType type)
{
    mOutputs[idx].mType = type;
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
        
        size_t unalignedSize = outs->mType == kFrameNormal ? outs->mRequestedSize * sizeof(double) : Serial::inPlaceSize(outs->mRequestedSize);
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
            
            if (outs->mType == kFrameTagged)
                Serial::newInPlace(outs->mMemory, outs->mCurrentSize);
        }
    
        return true;
    }
    
    // Reset outputs on failure of zero size
    
    for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
    {
        outs->mMemory = NULL;
        outs->mCurrentSize = 0;
    }
    
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

FrameLib_Parameters::Serial *FrameLib_DSP::getInput(unsigned long idx)
{
    if (mInputs[idx].mObject)
        return mInputs[idx].mObject->getOutput(mInputs[idx].mIndex);
    
    return NULL;
}

double *FrameLib_DSP::getOutput(unsigned long idx, size_t *size)
{
    if (mOutputs[0].mMemory && mOutputs[idx].mType == kFrameNormal)
    {
        *size = mOutputs[idx].mCurrentSize;
        return (double *) mOutputs[idx].mMemory;
    }
    
    *size = 0;
    return NULL;
}

FrameLib_Parameters::Serial *FrameLib_DSP::getOutput(unsigned long idx)
{
    if (mOutputs[0].mMemory && mOutputs[idx].mType == kFrameTagged)
        return (Serial *) mOutputs[idx].mMemory;
    
    return NULL;
}

// Dependency Notification

inline void FrameLib_DSP::dependencyNotify(bool releaseMemory, bool fromInput)
{
    if (releaseMemory)
        releaseOutputMemory();
    
    // If ready add to queue
    
    if (fromInput && mUpdatingInputs)
    {
        if (--mInputCount == 0)
           mQueue->add(this);
    }
    else if (--mDependencyCount == 0)
        mQueue->add(this);
    
    // N.B. For multithreading re-entrancy needs to be avoided by increasing the dependency count before adding to the queue (with matching notification)
}

// For updating the correct input count

void FrameLib_DSP::incrementInputDependency()
{
    if (mUpdatingInputs)
        mInputCount++;
    else
        mDependencyCount++;
}

// Main code to control time flow (called when all input/output dependencies are ready)

void FrameLib_DSP::dependenciesReady()
{
    
#ifndef NDEBUG
    FrameLib_TimeFormat prevInputTime = mInputTime;
#endif
    
    bool timeUpdated = false;
    bool callUpdate = false;
    
    // Check for inputs at the current frame time that update (update parameters if requested)
    
    for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
    {
        if (ins->mObject && ins->mUpdate && mInputTime == ins->mObject->mFrameTime)
        {
            callUpdate = true;
            if (ins->mParameters)
                mParameters.set(ins->mObject->getOutput(ins->mIndex));
        }
    }
    
    // Custom Update
    
    if (callUpdate)
    {
        mInUpdate = true;
        update();
        mInUpdate = false;
    }
    
    if (getType() == kScheduler)
    {
        // Find the input time (the min valid time of all inputs)
        
        mInputTime = FL_Limits<FrameLib_TimeFormat>::largest();
        
        for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
            if (ins->mObject && ins->mObject->mValidTime < mInputTime)
                mInputTime = ins->mObject->mValidTime;
        
        // Schedule
        
        bool upToDate = (mValidTime >= mBlockEndTime) || mUpdatingInputs;
        SchedulerInfo scheduleInfo = schedule(mOutputDone && !upToDate, upToDate);

        // Check if time has been updated (limiting to positive advances only), and if so set output times
                
        if ((timeUpdated = !upToDate && nonZeroPositive(scheduleInfo.mTimeAdvance)))
        {
            if (scheduleInfo.mNewFrame || mOutputDone)
            {
                setOutputDependencyCount();
                mFrameTime = mValidTime;
            }
            
            mValidTime += scheduleInfo.mTimeAdvance;
            mOutputDone = scheduleInfo.mOutputDone;            
        }
        
        // Revise the input time to take account of the end of the current frame (in order that we don't free anything we might still need)
        
        if (mValidTime < mInputTime)
            mInputTime = mValidTime;
    }
    else
    {
        // Find the valid till time (the min valid time of connected inputs that can trigger) and input time (the min valid time of all inputs)
        // Check for inputs at the current time that trigger (N.B. this is done after any update)

        FrameLib_TimeFormat prevValidTime = mValidTime;
        bool trigger = false;
        mInputTime = FL_Limits<FrameLib_TimeFormat>::largest();
        mValidTime = FL_Limits<FrameLib_TimeFormat>::largest();
    
        for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
        {
            if (ins->mObject && (ins->mTrigger || ins->mSwitchable) && ins->mObject->mValidTime < mValidTime)
                mValidTime = ins->mObject->mValidTime;
            if (ins->mObject && ins->mObject->mValidTime < mInputTime)
                mInputTime = ins->mObject->mValidTime;
            trigger |= (ins->mObject && ins->mTrigger && prevValidTime == ins->mObject->mFrameTime);
        }

        // If triggered update the frame time, process and release the inputs if we only have one dependency
        
        if (trigger)
        {
            mFrameTime = prevValidTime;
            process();
            setOutputDependencyCount();
            if (mInputDependencies.size() == 1)
                (*mInputDependencies.begin())->releaseOutputMemory();
        }
        
        // Check for the frame times updating and if so check for completion of the frame
        
        if (mValidTime != prevValidTime)
        {
            timeUpdated = true;
            mOutputDone = true;

            for (std::vector <Input>::iterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
            {
                if (ins->mObject && ((ins->mTrigger && !ins->mSwitchable) || (!ins->mObject->mOutputDone && ins->mSwitchable)) && (mValidTime == ins->mObject->mValidTime))
                {
                    if ((mOutputDone = ins->mObject->mOutputDone))
                        break;
                }
            }
        }
    }

    // Check for host alignment for objects requiring audio notification (treating the audio notification as a time dependency)

    bool hostAligned = requiresAudioNotification() && mInputTime >= mBlockEndTime;
    
    if (hostAligned)
        mInputTime = mBlockEndTime;
    
    // Check if we need to just updating inputs
    
    bool prevUpdatingInputs = mUpdatingInputs;
    mUpdatingInputs = mInputTime < mValidTime;
    
    // Increment the input dependency for the audio update if necessary (must be after we know if we are updating inputs only)
    
    if (hostAligned)
        incrementInputDependency();

    // Update dependency count for outputs and updating input state starting
    
    mDependencyCount += ((timeUpdated ? mOutputDependencies.size() : 0)) + ((mUpdatingInputs > prevUpdatingInputs) ? 1 : 0);
    
    // Notify input dependencies that can be released as they are up to date (releasing memory where relevant for objects with more than one input dependency)
    
    for (std::vector <FrameLib_DSP *>::iterator it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
    {
        if (mInputTime == (*it)->mValidTime)
        {
            incrementInputDependency();
            (*it)->dependencyNotify((getType() == kScheduler || mInputDependencies.size() != 1) && (*it)->mOutputDone, false);
        }
    }
    
    // If time has updated then notify output dependencies (of updates to their inputs)
    
    if (timeUpdated)
        for (std::vector <FrameLib_DSP *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            (*it)->dependencyNotify(false, true);
    
    // See if the updating input status has expired (must be done after resolving all other dependencies)
    
    if (mUpdatingInputs < prevUpdatingInputs)
        dependencyNotify(false, false);
    
    // Debug
    
    if (requiresAudioNotification())
        assert(prevInputTime >= mBlockStartTime && prevInputTime < mBlockEndTime && "Out of sync with host");
    assert(mInputTime > prevInputTime && "Failed to move time forward");
    assert(mInputTime <= mValidTime && "Inputs are ahead of output");
    assert(mFrameTime <= mInputTime && "Output is ahead of input dependencies");
}

void FrameLib_DSP::setOutputDependencyCount()
{
    mOutputMemoryCount = mOutputDependencies.size();
}

void FrameLib_DSP::resetDependencyCount()
{
    mUpdatingInputs = false;
    mInputCount = 0;
    mOutputMemoryCount = 0;
    mDependencyCount = mInputDependencies.size() + ((requiresAudioNotification()) ? 1 : 0);
    
    freeOutputMemory();
}

// Manage Output Memory

inline void FrameLib_DSP::freeOutputMemory()
{
    if (getNumOuts() && mOutputs[0].mMemory)
    {        
        // Call the destructor for any serial outputs
        
        for (std::vector <Output>::iterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
            if (outs->mType == kFrameTagged)
                ((Serial *)outs->mMemory)->Serial::~Serial();

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
    mInputs[inIdx].setInput();
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
            mInputs[i].setInput();

    // Update dependencies
    
    removeInputDependency(object);
    return object->removeOutputDependency(this);
}
