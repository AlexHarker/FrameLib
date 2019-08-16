
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

// Update the inputs of all output dependencies

void FrameLib_Multistream::outputUpdate(Queue *queue)
{
    addOutputDependencies(queue);
}
