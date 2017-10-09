
#include "FrameLib_DSP.h"

// Constructor / Destructor

FrameLib_DSP::FrameLib_DSP(ObjectType type, FrameLib_Context context, void *owner, FrameLib_Parameters::Info *info, unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans)
: FrameLib_Block(type, context, owner), mSamplingRate(44100.0), mMaxBlockSize(4096), mParameters(info), mProcessingQueue(context), mNext(NULL), mNoLiveInputs(true), mInUpdate(false)
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
    
    for (InputIterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
        delete[] ins->mFixedInput;
}

// Set Fixed Input

void FrameLib_DSP::setFixedInput(unsigned long idx, double *input, unsigned long size)
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
    
    LocalQueue(this, &FrameLib_DSP::reset);
}

void FrameLib_DSP::reset(LocalQueue *queue)
{
    bool prevNoLiveInputs = mNoLiveInputs;

    // Object specific reset
    
    objectReset();

    // Reset dependency counts
    
    mUpdatingInputs = false;
    mInputCount = 0;
    mOutputMemoryCount = 0;
    mDependencyCount = ((requiresAudioNotification()) ? 1 : 0);
    
    for (ObjectIterator it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
        if (!(*it)->mNoLiveInputs)
            mDependencyCount++;
    
    mNoLiveInputs = mDependencyCount == 0;

    // Remove info about the processing queue
    
    mNext = NULL;
    
    // Reset output
    
    freeOutputMemory();
    mOutputDone = false;

    // Reset times (Note that the first sample is 1 so that we can start the frames *before* this with non-negative values)

    mFrameTime = 0.0;
    mInputTime = mNoLiveInputs ? FL_Limits<FrameLib_TimeFormat>::largest() : FrameLib_TimeFormat(1.0);
    mValidTime = mNoLiveInputs ? FL_Limits<FrameLib_TimeFormat>::largest() : FrameLib_TimeFormat(1.0);
    mBlockStartTime = 1.0;
    mBlockEndTime = 1.0;
    
    // Update output dependencies for changes in live input status
    
    if (mNoLiveInputs != prevNoLiveInputs)
        for (ObjectIterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            queue->add(*it, &FrameLib_DSP::reset);
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

void FrameLib_DSP::setOutputMode(unsigned long idx, FrameType type)
{
    mOutputs[idx].mType = type;
    mOutputs[idx].mCurrentType = type != kFrameAny ? type : kFrameNormal;
    mOutputs[idx].mRequestedType = mOutputs[idx].mCurrentType;
}

// You should only call this from your process method (it is unsafe anywhere else)

void FrameLib_DSP::setCurrentOutputMode(unsigned long idx, FrameType type)
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
    
    for (OutputIterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
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
        size_t alignedSize = FrameLib_LocalAllocator::alignSize(unalignedSize);
        
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
        
        for (OutputIterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
        {
            outs->mMemory = pointer + outs->mPointerOffset;
            
            if (outs->mCurrentType == kFrameTagged)
                Serial::newInPlace(outs->mMemory, outs->mCurrentSize);
        }
    
        return true;
    }
    
    // Reset outputs on failure of zero size
    
    for (OutputIterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
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
    if (mOutputs[0].mMemory && mOutputs[idx].mCurrentType == kFrameNormal)
    {
        *size = mOutputs[idx].mCurrentSize;
        return (double *) mOutputs[idx].mMemory;
    }
    
    *size = 0;
    return NULL;
}

FrameLib_Parameters::Serial *FrameLib_DSP::getOutput(unsigned long idx)
{
    if (mOutputs[0].mMemory && mOutputs[idx].mCurrentType == kFrameTagged)
        return (Serial *) mOutputs[idx].mMemory;
    
    return NULL;
}

// Convience methods for copying and zeroing

void FrameLib_DSP::prepareCopyInputToOutput(unsigned long inIdx, unsigned long outIdx)
{
    FrameType requestType = mInputs[inIdx].getCurrentType();
    
    unsigned long size = 0;
    
    setCurrentOutputMode(outIdx, requestType);
    
    if (mInputs[inIdx].mObject)
    {
        if (requestType == kFrameNormal)
            getInput(inIdx, &size);
        else
            size = getInput(inIdx)->size();
    }
    
    requestOutputSize(outIdx, size);
}

void FrameLib_DSP::copyInputToOutput(unsigned long inIdx, unsigned long outIdx)
{
    if (mOutputs[outIdx].mCurrentType == kFrameNormal)
    {
        unsigned long inSize, outSize;
        
        double *input = getInput(inIdx, &inSize);
        double *output = getOutput(outIdx, &outSize);
        
        copyVector(output, input, std::min(inSize, outSize));
    }
    else
    {
        FrameLib_Parameters::Serial *output = getOutput(outIdx);
        output->write(getInput(inIdx));
    }
}

// Dependency Notification

inline void FrameLib_DSP::dependencyNotify(bool releaseMemory, bool fromInput)
{
    assert(((mDependencyCount > 0) || (mUpdatingInputs && (mInputCount > 0))) && "Dependency count is already zero");
    
    if (releaseMemory)
        releaseOutputMemory();
    
    // If ready add to queue
    
    if (fromInput && mUpdatingInputs)
    {
        if (--mInputCount == 0)
           mProcessingQueue->add(this);
    }
    else if (--mDependencyCount == 0 && !mUpdatingInputs)
        mProcessingQueue->add(this);
    
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
    
    mDependencyCount++;
    
    bool timeUpdated = false;
    bool callUpdate = false;
    
    // Check for inputs at the current frame time that update (update parameters if requested)
    
    for (InputIterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
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
        
        for (InputIterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
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
    
        for (InputIterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
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

            for (InputIterator ins = mInputs.begin(); ins != mInputs.end(); ins++)
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
    
    // Check if we have reached the end of time or need to just update inputs
    
    bool endOfTime = mInputTime == FL_Limits<FrameLib_TimeFormat>::largest();
    bool prevUpdatingInputs = mUpdatingInputs;
    mUpdatingInputs = mInputTime < mValidTime;
    
    // Increment the input dependency for the audio update if necessary (must be after we know if we are updating inputs only)
    
    if (hostAligned)
        incrementInputDependency();

    // Update dependency count for outputs and updating input state starting
    
    mDependencyCount += ((timeUpdated ? mOutputDependencies.size() : 0)) + ((mUpdatingInputs > prevUpdatingInputs) ? 1 : 0);
    
    // Notify input dependencies that can be released as they are up to date (releasing memory where relevant for objects with more than one input dependency)
    
    if (!endOfTime)
    {
        // Inputs cannot move beyond the end of time...
        
        for (ObjectIterator it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
        {
            if (mInputTime == (*it)->mValidTime)
            {
                incrementInputDependency();
                (*it)->dependencyNotify((getType() == kScheduler || mInputDependencies.size() != 1) && (*it)->mOutputDone, false);
            }
        }
    }
    
    // If time has updated then notify output dependencies (of updates to their inputs)
    
    if (timeUpdated)
        for (ObjectIterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            (*it)->dependencyNotify(false, true);
    
    // See if the updating input status has expired (must be done after resolving all other dependencies)
    
    if (mUpdatingInputs < prevUpdatingInputs)
        dependencyNotify(false, false);
    
    // Allow self-triggering if we haven't reached the end of time
    
    if (!endOfTime)
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

// Manage Output Memory

inline void FrameLib_DSP::freeOutputMemory()
{
    if (getNumOuts() && mOutputs[0].mMemory)
    {        
        // Call the destructor for any serial outputs
        
        for (OutputIterator outs = mOutputs.begin(); outs != mOutputs.end(); outs++)
            if (outs->mCurrentType == kFrameTagged)
                ((Serial *)outs->mMemory)->Serial::~Serial();

        // Then deallocate (will also set to NULL)
        
        dealloc(mOutputs[0].mMemory);
    }
}

inline void FrameLib_DSP::releaseOutputMemory()
{
    if (--mOutputMemoryCount == 0)
        freeOutputMemory();
}

// Connection Updating

void FrameLib_DSP::connectionUpdate(Queue *queue)
{
    ObjectIterator it;
    
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
    
    std::vector<FrameLib_Block *> dependencies = getOutputDependencies();
    
    for (std::vector<FrameLib_Block *>::iterator it = dependencies.begin(); it != dependencies.end(); it++)
    {
        FrameLib_DSP *object = dynamic_cast<FrameLib_DSP *>(*it);
        
        if (object)
            mOutputDependencies.push_back(object);
    }
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
        
    for (ObjectIterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
        queue->add(*it, &FrameLib_DSP::autoOrderingConnections);
}

void FrameLib_DSP::clearAutoOrderingConnections()
{
    callConnectionUpdate();
}
