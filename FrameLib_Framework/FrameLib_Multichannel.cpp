
#include "FrameLib_Multichannel.h"

// FrameLib_MultiChannel

// Query Connections for Individual Channels

unsigned long FrameLib_MultiChannel::getInputNumChans(unsigned long inIdx)
{
    MultiChannelConnection connection = getConnection(inIdx);
    
    return connection.mObject ? connection.mObject->mOutputs[connection.mIndex].size() : 0;
}

FrameLib_MultiChannel::BlockConnection FrameLib_MultiChannel::getInputChan(unsigned long inIdx, unsigned long chan)
{
    MultiChannelConnection connection = getConnection(inIdx);

    return connection.mObject->mOutputs[connection.mIndex][chan];
}

unsigned long FrameLib_MultiChannel::getOrderingConnectionNumChans(unsigned long idx)
{
    MultiChannelConnection connection = getOrderingConnection(idx);

    return connection.mObject ? connection.mObject->mOutputs[connection.mIndex].size() : 0;
}

FrameLib_MultiChannel::BlockConnection FrameLib_MultiChannel::getOrderingConnectionChan(unsigned long idx, unsigned long chan)
{
    MultiChannelConnection connection = getOrderingConnection(idx);
    
    return connection.mObject->mOutputs[connection.mIndex][chan];
}

// Update the inputs of all output dependencies

void FrameLib_MultiChannel::outputUpdate(Queue *queue)
{
    addOutputDependencies(queue);
}

// ************************************************************************************** //

// FrameLib_Pack - Pack Multichannel signals

FrameLib_Pack::ParameterInfo FrameLib_Pack::sParamInfo;

FrameLib_Pack::FrameLib_Pack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner, unsigned long nStreams): FrameLib_MultiChannel(kProcessor, context, owner, 1), mParameters(&sParamInfo)
{
    mParameters.addInt(0, "inputs", 2, 0 );
    mParameters.setInstantiation();
    mParameters.set(serialisedParameters);
    setIO(mParameters.getValue(kInputs), 1);
}

std::string FrameLib_Pack::objectInfo(bool verbose)
{
    return formatInfo("Packs multiple frame streams into a multichannel output: Inputs may be single or multichannel. All inputs are concatenated in order, with blank inputs ignored.",
                   "Packs multiple frame streams into a multichannel output.", verbose);
}

std::string FrameLib_Pack::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input # - may be single or multichannel", "Input #", idx, verbose);
}

std::string FrameLib_Pack::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output - packed multichannel connection consisting of all input channels", "Multichannel Output", idx, verbose);
}

bool FrameLib_Pack::inputUpdate()
{
    mOutputs[0].clear();
    
    for (unsigned long i = 0; i < getNumIns(); i++)
        for (unsigned long j = 0; j < getInputNumChans(i); j++)
            mOutputs[0].push_back(getInputChan(i, j));
    
    return true;
}

// ************************************************************************************** //

// FrameLib_Unpack - Unpack Multichannel Signals

FrameLib_Unpack::ParameterInfo FrameLib_Unpack::sParamInfo;

FrameLib_Unpack::FrameLib_Unpack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner, unsigned long nStreams) : FrameLib_MultiChannel(kProcessor, context, owner, 1), mParameters(&sParamInfo)
{
    mParameters.addInt(kOutputs, "outputs", 2, 0);
    mParameters.setInstantiation();
    mParameters.set(serialisedParameters);
    setIO(1, mParameters.getValue(kOutputs));    
}

std::string FrameLib_Unpack::objectInfo(bool verbose)
{
    return formatInfo("Unpacks multichannel input into single channel frames streams: Multichannel inputs are unpacked in order across the outputs.",
                   "Unpacks multichannel input into single channel frames streams.", verbose);
}

std::string FrameLib_Unpack::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Multichannel Input - to be unpacked into single channels", "Multichannel Input", idx, verbose);
}

std::string FrameLib_Unpack::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output # - single channel", "Output #", idx, verbose);
}

bool FrameLib_Unpack::inputUpdate()
{
    for (unsigned long i = 0; i < getNumOuts(); i++)
        mOutputs[i].clear();
    
    for (unsigned long i = 0; i < getInputNumChans(0) && i < getNumOuts(); i++)
        mOutputs[i].push_back(getInputChan(0, i));
    
    return true;
}
