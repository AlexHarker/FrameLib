
#include "FrameLib_DSP.h"

// Constructor / Destructor

FrameLib_DSP::FrameLib_DSP(ObjectType type, FrameLib_Context context, FrameLib_Proxy *proxy, FrameLib_Parameters::Info *info, unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans)
: FrameLib_Block(type, context, proxy)
, mSamplingRate(44100.0)
, mMaxBlockSize(4096)
, mParameters(context, proxy, info)
, mProcessingQueue(context)
, mNoLiveInputs(true)
, mInUpdate(false)
{    
    // Set IO
    
    setIO(nIns, nOuts, nAudioChans);
}

// Destructor

FrameLib_DSP::~FrameLib_DSP()
{
    // Free output
    
    freeOutputMemory();
    
    // Delete fixed inputs
    
    for (auto ins = mInputs.begin(); ins != mInputs.end(); ins++)
        delete[] ins->mFixedInput;
}

// Set Fixed Input

void FrameLib_DSP::setFixedInput(unsigned long idx, const double *input, unsigned long size)
{
    // This is not threadsafe with the processing thread and should not be called concurrently...
    
    mInputs[idx].mSize = 0;
    
    if (mInputs[idx].mFixedInput)
        delete[] mInputs[idx].mFixedInput;
    
    mInputs[idx].mFixedInput = new double[size];
    
    if (mInputs[idx].mFixedInput)
    {
        copyVector(mInputs[idx].mFixedInput, input, size);
        mInputs[idx].mSize = size;
    }
}

const double *FrameLib_DSP::getFixedInput(unsigned long idx, unsigned long *size)
{
    *size = mInputs[idx].mSize;
    return mInputs[idx].mFixedInput;
}

// Audio Processing

// Block updates for objects with audio IO (calls through to the group notifier version)

void FrameLib_DSP::blockUpdate(const double * const *ins, double **outs, unsigned long blockSize)
{
    FrameLib_AudioQueue queue;
    blockUpdate(ins, outs, blockSize, queue);
}

// This version is usable for making block notifcations together, prinarily in FrameLib_Expand

void FrameLib_DSP::blockUpdate(const double * const *ins, double **outs, unsigned long blockSize, FrameLib_AudioQueue& queue)
{
    // Update block time and process the block
    
    mBlockStartTime = mBlockEndTime;
    mBlockEndTime += blockSize;
    blockProcess(ins, outs, blockSize);
    
    // If the object is handling audio updates (but is not an output object) then notify
    
    if (needsAudioNotification())
    {
        queue.setUser(this);
        dependencyNotify(kAudioBlock, false, nullptr, queue);
    }
}

// Reset

void FrameLib_DSP::reset(double samplingRate, unsigned long maxBlockSize)
{
    // Store sample rate / max block size and call object specific reset
    
    mSamplingRate = samplingRate > 0 ? samplingRate : 44100.0;
    mMaxBlockSize = maxBlockSize;
    
    LocalQueue(this, &FrameLib_DSP::reset);
    
    mProcessingQueue->reset();
}

void FrameLib_DSP::reset(LocalQueue *queue)
{
    bool prevNoLiveInputs = mNoLiveInputs;

    // Object specific reset
    
    objectReset();

    // Reset dependency counts
    
    mUpdatingInputs = false;
    mInputCount = 0;
    mDependencyCount = ((needsAudioNotification()) ? 1 : 0);
    
    resetOutputDependencyCount();
    
    for (auto it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
        if (!(*it)->mNoLiveInputs)
            mDependencyCount++;
    
    mNoLiveInputs = mDependencyCount == 0;

    // Reset output
    
    freeOutputMemory();
    mOutputDone = false;

    // Reset times (Note that the first sample is 1 so that we can start the frames *before* this with non-negative values)

    mFrameTime = 0.0;
    mInputTime = mNoLiveInputs ? FrameLib_TimeFormat::largest() : FrameLib_TimeFormat(1.0);
    mValidTime = mNoLiveInputs ? FrameLib_TimeFormat::largest() : FrameLib_TimeFormat(1.0);
    mBlockStartTime = 1.0;
    mBlockEndTime = 1.0;
    
    // Update output dependencies for changes in live input status
    
    if (mNoLiveInputs != prevNoLiveInputs)
        for (auto it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            queue->add(*it);
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

void FrameLib_DSP::setInputMode(unsigned long idx, bool update, bool trigger, bool switchable, FrameType type)
{
    mInputs[idx].mUpdate = update;
    mInputs[idx].mTrigger = trigger;
    mInputs[idx].mSwitchable = switchable;
    mInputs[idx].mType = type;
}

// Call this from your constructor only (unsafe elsewhere)

void FrameLib_DSP::setParameterInput(unsigned long idx)
{
    setInputMode(idx, true, false, false, kFrameTagged);
    mInputs[idx].mParameters = true;
}

// Call this from your constructor only (unsafe elsewhere)

void FrameLib_DSP::addParameterInput()
{
    unsigned long nIns = getNumIns();
    unsigned long nOuts = getNumOuts();
    unsigned long nAudioChans = getNumAudioChans();
    
    setIO(nIns + 1, nOuts, nAudioChans);
    setParameterInput(nIns);
}

// Call this from your constructor only (unsafe elsewhere)

void FrameLib_DSP::setOutputType(unsigned long idx, FrameType type)
{
    mOutputs[idx].mType = type;
    mOutputs[idx].mCurrentType = type != kFrameAny ? type : kFrameNormal;
    mOutputs[idx].mRequestedType = mOutputs[idx].mCurrentType;
}

// You should only call this from your process method (it is unsafe anywhere else)

void FrameLib_DSP::setCurrentOutputType(unsigned long idx, FrameType type)
{
    mOutputs[idx].mRequestedType = type;
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
    
    for (auto outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
    {
        // Update type
        
        if (outs->mRequestedType != outs->mCurrentType)
        {
            if (outs->mType == kFrameAny)
                outs->mCurrentType = outs->mRequestedType;
            else
                outs->mRequestedSize = 0;
        }
        
        // Calculate allocation size, including necessary alignment padding and assuming success
        
        size_t unalignedSize = outs->mCurrentType == kFrameNormal ? outs->mRequestedSize * sizeof(double) : Serial::inPlaceSize(outs->mRequestedSize);
        size_t alignedSize = FrameLib_ContextAllocator::alignSize(unalignedSize);
        
        outs->mCurrentSize = outs->mRequestedSize;
        outs->mPointerOffset = allocationSize;
        
        allocationSize += alignedSize;
    }
    
    // Free then allocate memory
    
    freeOutputMemory();
    BytePointer pointer = alloc<Byte>(allocationSize);
    
    if (pointer)
    {
        // Store pointers and create tagged outputs
        
        for (auto outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
        {
            outs->mMemory = pointer + outs->mPointerOffset;
            
            if (outs->mCurrentType == kFrameTagged)
                Serial::newInPlace(outs->mMemory, outs->mCurrentSize);
        }
    
        return true;
    }
    
    // Reset outputs on failure or zero size
    
    for (auto outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
    {
        outs->mMemory = nullptr;
        outs->mCurrentSize = 0;
    }
    
    return false;
}

// Get Inputs and Outputs

const double *FrameLib_DSP::getInput(unsigned long idx, unsigned long *size) const
{
    if (mInputs[idx].mObject)
        return mInputs[idx].mObject->getOutput(mInputs[idx].mIndex, size);
    
    *size = mInputs[idx].mSize;
    return mInputs[idx].mFixedInput;
}

const FrameLib_Parameters::Serial *FrameLib_DSP::getInput(unsigned long idx) const
{
    if (mInputs[idx].mObject)
        return mInputs[idx].mObject->getOutput(mInputs[idx].mIndex);
    
    return nullptr;
}

double *FrameLib_DSP::getOutput(unsigned long idx, unsigned long *size) const
{
    if (mOutputs[0].mMemory && mOutputs[idx].mCurrentType == kFrameNormal)
    {
        *size = mOutputs[idx].mCurrentSize;
        return (double *) mOutputs[idx].mMemory;
    }
    
    *size = 0;
    return nullptr;
}

FrameLib_Parameters::Serial *FrameLib_DSP::getOutput(unsigned long idx) const
{
    if (mOutputs[0].mMemory && mOutputs[idx].mCurrentType == kFrameTagged)
        return (Serial *) mOutputs[idx].mMemory;
    
    return nullptr;
}

// Convience methods for copying and zeroing

void FrameLib_DSP::prepareCopyInputToOutput(unsigned long inIdx, unsigned long outIdx)
{
    FrameType requestType = mInputs[inIdx].getCurrentType();
    
    unsigned long size = 0;
    
    setCurrentOutputType(outIdx, requestType);
    
    if (requestType == kFrameNormal)
        getInput(inIdx, &size);
    else
        size = getInput(inIdx)->size();
    
    requestOutputSize(outIdx, size);
}

void FrameLib_DSP::copyInputToOutput(unsigned long inIdx, unsigned long outIdx)
{
    if (mOutputs[outIdx].mCurrentType == kFrameNormal)
    {
        unsigned long inSize, outSize;
        
        const double *input = getInput(inIdx, &inSize);
        double *output = getOutput(outIdx, &outSize);
        
        copyVector(output, input, std::min(inSize, outSize));
    }
    else
    {
        FrameLib_Parameters::Serial *output = getOutput(outIdx);
        if (output)
            output->write(getInput(inIdx));
    }
}

// Dependency Notification

bool FrameLib_DSP::dependencyNotify(NotificationType type, bool releaseMemory, LocalAllocator *allocator)
{
    if (mProcessingQueue->isTimedOut())
        return false;
    
    assert(((mDependencyCount > 0) || (mUpdatingInputs && (mInputCount > 0))) && "Dependency count is already zero");
    
    if (releaseMemory)
        releaseOutputMemory(allocator);
    
    bool useInputCount = mUpdatingInputs && (type == kInputConnection || type == kAudioBlock);
    
    if ((useInputCount && --mInputCount == 0) || (!useInputCount && --mDependencyCount == 0))
    {
        // N.B. Avoid re-entrancy by increasing the dependency count before processing (plus matched notification)

        assert((mDependencyCount > 0) || !mUpdatingInputs && "Dependency count shouldn't be zero if updating inputs");
        mDependencyCount++;
        return true;
    }
    
    return false;
}

void FrameLib_DSP::dependencyNotify(NotificationType type, bool releaseMemory, LocalAllocator *allocator, NotificationQueue &queue)
{
    // If ready then this item should be added to the processing queue

    if (dependencyNotify(type, releaseMemory, allocator))
        queue.push(this);
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

void FrameLib_DSP::dependenciesReady(LocalAllocator *allocator)
{
    setLocalAllocator(allocator);
    
#ifndef NDEBUG
    FrameLib_TimeFormat inputTime = mInputTime;
#endif
    
    bool timeUpdated = false;
    bool callUpdate = false;
    
    // Check for inputs at the current frame time that update (update parameters if requested)
    
    for (auto ins = mInputs.begin(); ins != mInputs.end(); ins++)
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
        
        mInputTime = FrameLib_TimeFormat::largest();
        
        for (auto ins = mInputs.begin(); ins != mInputs.end(); ins++)
            if (ins->mObject && ins->mObject->mValidTime < mInputTime)
                mInputTime = ins->mObject->mValidTime;
        
        // Schedule
        
        bool upToDate = (mValidTime >= mBlockEndTime) || mUpdatingInputs;
        SchedulerInfo scheduleInfo = schedule(mOutputDone && !upToDate, upToDate);

        // Check if time has been updated (limiting to positive advances only), and if so set output times
                
        if ((timeUpdated = !upToDate && scheduleInfo.mTimeAdvance.greaterThanZero()))
        {
            if (scheduleInfo.mNewFrame || mOutputDone)
            {
                resetOutputDependencyCount();
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
        mInputTime = FrameLib_TimeFormat::largest();
        mValidTime = FrameLib_TimeFormat::largest();
    
        for (auto ins = mInputs.begin(); ins != mInputs.end(); ins++)
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
            resetOutputDependencyCount();
            if (mInputDependencies.size() == 1)
                (*mInputDependencies.begin())->releaseOutputMemory(allocator);
        }
        
        // Check for the frame times updating and if so check for completion of the frame
        
        if (mValidTime != prevValidTime)
        {
            timeUpdated = true;
            mOutputDone = false;

            for (auto ins = mInputs.begin(); ins != mInputs.end(); ins++)
            {
                if (ins->mObject && (ins->mTrigger && !ins->mSwitchable) && (mValidTime == ins->mObject->mValidTime))
                {
                    if ((mOutputDone = ins->mObject->mOutputDone))
                        break;
                }
            }
        }
    }

    // Check for host alignment for objects needing audio notification (treating the audio notification as a time dependency)

    bool hostAligned = needsAudioNotification() && mInputTime >= mBlockEndTime;
    
    if (hostAligned)
        mInputTime = mBlockEndTime;
    
    // Check if we have reached the end of time or need to just update inputs
    
    bool endOfTime = mInputTime == FrameLib_TimeFormat::largest();
    bool prevUpdatingInputs = mUpdatingInputs;
    mUpdatingInputs = mInputTime < mValidTime;
    
    // Increment  input dependency for the audio update if needed (must be after we know if we are updating inputs only)
    
    if (hostAligned)
        incrementInputDependency();

    // Update dependency count for outputs and updating input state starting
    
    mDependencyCount += ((timeUpdated ? getNumOuputDependencies() : 0)) + ((mUpdatingInputs > prevUpdatingInputs) ? 1 : 0);
    
    // Notify input dependencies that can be released as they are up to date (releasing memory where relevant for objects with more than one input dependency)
    
    NotificationQueue queue;
    
    if (!endOfTime)
    {
        // Inputs cannot move beyond the end of time...
        
        for (auto it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
        {
            if (mInputTime == (*it)->mValidTime)
            {
                incrementInputDependency();
                (*it)->dependencyNotify(kOutputConnection, (getType() == kScheduler || mInputDependencies.size() != 1) && (*it)->mOutputDone, allocator, queue);
            }
        }
    }
    
    // If time has updated then notify output dependencies (of updates to their inputs)
    
    if (timeUpdated)
        for (auto it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            (*it)->dependencyNotify(kInputConnection, false, allocator, queue);
    
    removeLocalAllocator();

    // See if the updating input status has expired (must be done after resolving all other dependencies)
    
    if (mUpdatingInputs < prevUpdatingInputs)
        dependencyNotify(kSelfConnection, false, allocator, queue);
    
    // Debug (before re-entering)
    
    assert(!needsAudioNotification() || (inputTime >= mBlockStartTime && inputTime < mBlockEndTime) && "Out of sync with host");
    assert(mInputTime > inputTime && "Failed to move time forward");
    assert(mInputTime <= mValidTime && "Inputs are ahead of output");
    assert(mFrameTime <= mInputTime && "Output is ahead of input dependencies");
    
    // Allow self-triggering if we haven't reached the end of time
    
    if (!endOfTime)
        dependencyNotify(kSelfConnection, false, allocator, queue);
    
    mProcessingQueue->add(queue, this);
}

void FrameLib_DSP::resetOutputDependencyCount()
{
    mOutputMemoryCount = getNumOuputDependencies();
}

// Manage Output Memory

inline void FrameLib_DSP::freeOutputMemory()
{
    if (getNumOuts() && mOutputs[0].mMemory)
    {        
        // Call the destructor for any serial outputs
        
        for (auto outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
            if (outs->mCurrentType == kFrameTagged)
                ((Serial *)outs->mMemory)->Serial::~Serial();

        // Then deallocate (will also set to nullptr)
        
        dealloc(mOutputs[0].mMemory);
    }
}

inline void FrameLib_DSP::releaseOutputMemory(LocalAllocator *allocator)
{
    assert(mOutputMemoryCount > 0 && "Output memory count is already zero");
    
    if (--mOutputMemoryCount == 0)
    {
        setLocalAllocator(allocator);
        freeOutputMemory();
        removeLocalAllocator();
    }
}

// Connection Updating

void FrameLib_DSP::connectionUpdate(BlockQueue *queue)
{
    std::vector<FrameLib_DSP *>::iterator it;
    
    // Clear dependencies
    
    mInputDependencies.clear();
    mOutputDependencies.clear();

    // Build the input list
    
    mInputs.resize(getNumIns() + getNumOrderingConnections());

    for (unsigned long i = 0; i < getNumIns() + getNumOrderingConnections(); i++)
    {
        // Make sure that ordering inputs are set correctly
        
        if (i >= getNumIns())
            setInputMode(i, false, false, false);

        // Add the DSP object connection details to the input
        
        Connection connection = i < getNumIns() ? getConnectionInternal(i) : getOrderingConnectionInternal(i - getNumIns());
        
        mInputs[i].mObject = dynamic_cast<FrameLib_DSP *>(connection.mObject);
        mInputs[i].mIndex = mInputs[i].mObject ? connection.mIndex : 0;

        // Build the input dependency list
        
        if (mInputs[i].mObject)
            addUniqueItem(mInputDependencies, mInputs[i].mObject);
    }
    
    // Build the output dependency list
    
    addOutputDependencies(mOutputDependencies);
}

void FrameLib_DSP::autoOrderingConnections()
{
     if (supportsOrderingConnections())
         LocalQueue(this, &FrameLib_DSP::autoOrderingConnections);
}

void FrameLib_DSP::autoOrderingConnections(LocalQueue *queue)
{
    if (supportsOrderingConnections() && queue->getFirst())
        addOrderingConnection(Connection(queue->getFirst(), 0));
        
    for (auto it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
        queue->add(*it);
}

void FrameLib_DSP::clearAutoOrderingConnections()
{
    callConnectionUpdate();
}
