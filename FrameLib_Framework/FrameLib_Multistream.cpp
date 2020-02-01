
#include "FrameLib_Multistream.h"

// FrameLib_Multistream

// Query Connections for Individual Channels

unsigned long FrameLib_Multistream::getInputNumChans(unsigned long inIdx)
{
    MultistreamConnection connection = getConnection(inIdx);
    
    if (connection.mObject)
        return static_cast<unsigned long>(connection.mObject->mOutputs[connection.mIndex].size());
    
    return 0;
}

FrameLib_Multistream::BlockConnection FrameLib_Multistream::getInputChan(unsigned long inIdx, unsigned long chan)
{
    MultistreamConnection connection = getConnection(inIdx);

    return connection.mObject->mOutputs[connection.mIndex][chan];
}

unsigned long FrameLib_Multistream::getOrderingConnectionNumChans(unsigned long idx)
{
    MultistreamConnection connection = getOrderingConnection(idx);

    if (connection.mObject)
        return static_cast<unsigned long>(connection.mObject->mOutputs[connection.mIndex].size());
    
    return 0;
}

FrameLib_Multistream::BlockConnection FrameLib_Multistream::getOrderingConnectionChan(unsigned long idx, unsigned long chan)
{
    MultistreamConnection connection = getOrderingConnection(idx);
    
    return connection.mObject->mOutputs[connection.mIndex][chan];
}

// Update connections

void FrameLib_Multistream::connectionUpdate(Queue *queue)
{
    InputStack stack;
    
    for (FrameLib_Multistream *object = this; object; object = stack.pop())
        object->inputCheck(stack);
    
    if (mOutputChange)
        outputUpdate(queue);
    
    mOutputChange = false;
}

// Update the inputs of all output dependencies

void FrameLib_Multistream::outputUpdate(Queue *queue)
{
    addOutputDependencies(queue);
}

// Check all the inputs are valid

void FrameLib_Multistream::inputCheck(InputStack& stack)
{
    auto checkInputs = [&](const MultistreamConnection& connection)
    {
        if (!connection.mObject || connection.mObject->mOwnsStreams)
            return false;
        
        stack.push(this);
        stack.push(connection.mObject);
        return true;
    };
    
    // Ensure that all inputs are valid for normal and ordering connections
    
    while (mInCount < getNumIns())
        if (checkInputs(getConnection(mInCount++)))
            return;
    
    while (mInCount < getNumIns() + getNumOrderingConnections())
        if (checkInputs(getOrderingConnection(mInCount++ - getNumIns())))
            return;
    
    mInCount = 0;
    mOutputChange = inputUpdate();
}
