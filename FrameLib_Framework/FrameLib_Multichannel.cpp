
#include "FrameLib_Multichannel.h"

// FrameLib_MultiChannel

// This abstract class allows mulitchannel connnections and the means to update the network according to the number of channels

void FrameLib_MultiChannel::outputUpdate()
{
    // Add to the queue to update all output dependencies
    
    if (mQueue)
        for (std::vector <FrameLib_MultiChannel *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            mQueue->add(*it);
}

unsigned long FrameLib_MultiChannel::getNumChans(unsigned long inIdx)
{
    if (mInputs[inIdx].mObject)
        return mInputs[inIdx].mObject->mOutputs[mInputs[inIdx].mIndex].mConnections.size();
    
    return 0;
}

FrameLib_MultiChannel::ConnectionInfo FrameLib_MultiChannel::getChan(unsigned long inIdx, unsigned long chan)
{
    if (mInputs[inIdx].mObject == NULL)
    {
        unsigned long *ptr = NULL;
        *ptr = 0;
        
    }
    
    return mInputs[inIdx].mObject->mOutputs[mInputs[inIdx].mIndex].mConnections[chan];
}

// ************************************************************************************** //

// Connection methods (private)

// Dependency updating

std::vector <FrameLib_MultiChannel *>::iterator FrameLib_MultiChannel::removeOutputDependency(FrameLib_MultiChannel *object)
{
    std::vector <FrameLib_MultiChannel *>::iterator it;
    
    for (it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
        if (*it == object)
            return mOutputDependencies.erase(it);
    
    return it;
}

void FrameLib_MultiChannel::addOutputDependency(FrameLib_MultiChannel *object)
{
    for (std::vector <FrameLib_MultiChannel *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
        if (*it == object)
            return;
    
    mOutputDependencies.push_back(object);
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

std::vector <FrameLib_MultiChannel *>::iterator FrameLib_MultiChannel::removeConnections(FrameLib_MultiChannel *object)
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
    
    std::vector <FrameLib_MultiChannel *>::iterator updatedIterator = object->removeOutputDependency(this);
    if (mQueue)
        mQueue->add(this);
    
    return updatedIterator;
}

// ************************************************************************************** //

// Connection methods (public)

// N.B. - No sanity checks here to maximise speed and help debugging (better for it to crash if a mistake is made)

void FrameLib_MultiChannel::deleteConnection(unsigned long inIdx)
{
    // Update Dependencies
    
    removeConnection(inIdx);
    
    // Set default values
    
    mInputs[inIdx].mObject = NULL;
    mInputs[inIdx].mIndex = 0;
    
    // Update
    
    if (mQueue)
        mQueue->add(this);
}

void FrameLib_MultiChannel::addConnection(FrameLib_MultiChannel *object, unsigned long outIdx, unsigned long inIdx)
{
    // Update dependencies if the connected object has changed
    
    if (mInputs[inIdx].mObject != object)
    {
        removeConnection(inIdx);
        object->addOutputDependency(this);
    }
    
    // Store data about connection
    
    mInputs[inIdx].mObject = object;
    mInputs[inIdx].mIndex = outIdx;
    
    // Update
    
    if (mQueue)
        mQueue->add(this);
}

void FrameLib_MultiChannel::clearConnections()
{
    // Remove input connections
    
    for (unsigned long i = 0; i < mInputs.size(); i++)
    {
        removeConnection(i);
        mInputs[i].mObject = NULL;
        mInputs[i].mIndex = 0;
    }
    
    // Update
    
    if (mQueue)
        mQueue->add(this);
    
    // Remove output connections
    
    for (std::vector <FrameLib_MultiChannel *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); )
        it = (*it)->removeConnections(this);
}

bool FrameLib_MultiChannel::isConnected(unsigned long inIdx)
{
    return mInputs[inIdx].mObject != NULL;
}

// ************************************************************************************** //

// FrameLib_Pack - Pack single multichannel signals

FrameLib_Pack::FrameLib_Pack(FrameLib_Context context, FrameLib_Attributes::Serial *serialAttributes, void *owner) : FrameLib_MultiChannel(context)
{
    mAttributes.addDouble(0, "inputs", 2, 0 );
    mAttributes.set(serialAttributes);
    setIO(mAttributes.getValue(kInputs), 1);
}

bool FrameLib_Pack::inputUpdate()
{
    mOutputs[0].mConnections.clear();
    
    for (unsigned long i = 0; i < getNumIns(); i++)
        for (unsigned long j = 0; j < getNumChans(i); j++)
            mOutputs[0].mConnections.push_back(getChan(i, j));
    
    return true;
}

// ************************************************************************************** //

// FrameLib_Unpack - Unpack multichannel signals

FrameLib_Unpack::FrameLib_Unpack(FrameLib_Context context, FrameLib_Attributes::Serial *serialAttributes, void *owner) : FrameLib_MultiChannel(context)
{
    mAttributes.addDouble(kOutputs, "outputs", 2, 0);
    mAttributes.set(serialAttributes);
    setIO(1, mAttributes.getValue(kOutputs));
}

bool FrameLib_Unpack::inputUpdate()
{
    for (unsigned long i = 0; i < getNumOuts(); i++)
        mOutputs[i].mConnections.clear();
    
    for (unsigned long i = 0; i < getNumChans(0) && i < getNumOuts(); i++)
        mOutputs[i].mConnections.push_back(getChan(0, i));
    
    return true;
}
