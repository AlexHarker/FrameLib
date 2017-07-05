
#include "FrameLib_Multichannel.h"

// FrameLib_MultiChannel

// Connection methods (public)

void FrameLib_MultiChannel::deleteConnection(unsigned long inIdx)
{
    clearConnection(inIdx);
    updateConnections();
}

void FrameLib_MultiChannel::addConnection(FrameLib_MultiChannel *object, unsigned long outIdx, unsigned long inIdx)
{
    // Update dependencies if the connected object has changed
    
    if (mInputs[inIdx].mObject != object)
    {
        removeConnection(inIdx);
        object->addOutputDependency(this);
    }
    
    // Store data about connection and update
    
    mInputs[inIdx] = MultiChannelInput(object, outIdx);
    updateConnections();
}

void FrameLib_MultiChannel::clearConnections()
{
    // Remove input connections and update
    
    for (unsigned long i = 0; i < mInputs.size(); i++)
        clearConnection(i);
    
    // Remove output connections
    
    for (std::vector <FrameLib_MultiChannel *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); )
        it = (*it)->disconnect(this);
    
    // Update
    
    updateConnections();
}

bool FrameLib_MultiChannel::isConnected(unsigned long inIdx)
{
    return mInputs[inIdx].mObject != NULL;
}

unsigned long FrameLib_MultiChannel::getInputNumChans(unsigned long inIdx)
{
    if (mInputs[inIdx].mObject)
        return mInputs[inIdx].mObject->mOutputs[mInputs[inIdx].mIndex].mConnections.size();
    
    return 0;
}

// Dependency updating

void FrameLib_MultiChannel::addOutputDependency(FrameLib_MultiChannel *object)
{
    for (std::vector <FrameLib_MultiChannel *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
        if (*it == object)
            return;
    
    mOutputDependencies.push_back(object);
}

std::vector <FrameLib_MultiChannel *>::iterator FrameLib_MultiChannel::removeOutputDependency(FrameLib_MultiChannel *object)
{
    std::vector <FrameLib_MultiChannel *>::iterator it;
    
    for (it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
        if (*it == object)
            return mOutputDependencies.erase(it);
    
    return it;
}

// Connection Methods (private)

// Remove connection and set to defaults

void FrameLib_MultiChannel::clearConnection(unsigned long inIdx)
{
    removeConnection(inIdx);
    mInputs[inIdx] = MultiChannelInput();
}

// Removal of one connection to this object (before replacement / deletion)

void FrameLib_MultiChannel::removeConnection(unsigned long inIdx)
{
    // Check that there is an object connected and that it is not connected to another input also
    
    if (!mInputs[inIdx].mObject)
        return;
    
    for (unsigned long i = 0; i < mInputs.size(); i++)
        if (mInputs[i].mObject == mInputs[inIdx].mObject && i != inIdx)
            return;
    
    // Update dependencies
    
    mInputs[inIdx].mObject->removeOutputDependency(this);
}

// Removal of all connections from one object to this object

std::vector <FrameLib_MultiChannel *>::iterator FrameLib_MultiChannel::disconnect(FrameLib_MultiChannel *object)
{
    // Set any inputs connected to the object to default values
    
    for (unsigned long i = 0; i < mInputs.size(); i++)
        if (mInputs[i].mObject == object)
            mInputs[i] = MultiChannelInput();
    
    // Update dependencies
    
    std::vector <FrameLib_MultiChannel *>::iterator updatedIterator = object->removeOutputDependency(this);
    updateConnections();
    
    return updatedIterator;
}

// Update the inputs of all output dependencies

void FrameLib_MultiChannel::outputUpdate()
{    
    if (mQueue)
        for (std::vector <FrameLib_MultiChannel *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            mQueue->add(*it);
}

// ************************************************************************************** //

// FrameLib_Pack - Pack single multichannel signals

FrameLib_Pack::FrameLib_Pack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_MultiChannel(context)
{
    mParameters.addDouble(0, "inputs", 2, 0 );
    mParameters.set(serialisedParameters);
    setIO(mParameters.getValue(kInputs), 1);
}

bool FrameLib_Pack::inputUpdate()
{
    mOutputs[0].mConnections.clear();
    
    for (unsigned long i = 0; i < getNumIns(); i++)
        for (unsigned long j = 0; j < getInputNumChans(i); j++)
            mOutputs[0].mConnections.push_back(getInputChan(i, j));
    
    return true;
}

// ************************************************************************************** //

// FrameLib_Unpack - Unpack multichannel signals

FrameLib_Unpack::FrameLib_Unpack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_MultiChannel(context)
{
    mParameters.addDouble(kOutputs, "outputs", 2, 0);
    mParameters.set(serialisedParameters);
    setIO(1, mParameters.getValue(kOutputs));
}

bool FrameLib_Unpack::inputUpdate()
{
    for (unsigned long i = 0; i < getNumOuts(); i++)
        mOutputs[i].mConnections.clear();
    
    for (unsigned long i = 0; i < getInputNumChans(0) && i < getNumOuts(); i++)
        mOutputs[i].mConnections.push_back(getInputChan(0, i));
    
    return true;
}
