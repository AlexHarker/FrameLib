
#include "FrameLib_Multichannel.h"

// FrameLib_MultiChannel

// Query Input Channels

unsigned long FrameLib_MultiChannel::getInputNumChans(unsigned long inIdx)
{
    if (getConnection(inIdx))
        return getConnection(inIdx)->mOutputs[getConnectionIdx(inIdx)].mConnections.size();
    
    return 0;
}

FrameLib_MultiChannel::ConnectionInfo FrameLib_MultiChannel::getInputChan(unsigned long inIdx, unsigned long chan)
{
    return getConnection(inIdx)->mOutputs[getConnectionIdx(inIdx)].mConnections[chan];
}

unsigned long FrameLib_MultiChannel::getOrderingConnectionNumChans(unsigned long idx)
{
    if (getOrderingConnection(idx))
        return getOrderingConnection(idx)->mOutputs[getOrderingConnectionIdx(idx)].mConnections.size();
    
    return 0;
}

FrameLib_MultiChannel::ConnectionInfo FrameLib_MultiChannel::getOrderingConnectionChan(unsigned long idx, unsigned long chan)
{
    return getOrderingConnection(idx)->mOutputs[getOrderingConnectionIdx(idx)].mConnections[chan];
}

// Update the inputs of all output dependencies

void FrameLib_MultiChannel::outputUpdate(Queue *queue)
{    
    for (unsigned long i = 0; i < getNumOutputDependencies(); i++)
        queue->add(getOutputDependency(i), &FrameLib_MultiChannel::connectionUpdate);
}

// ************************************************************************************** //

// FrameLib_Pack - Pack Multichannel signals

FrameLib_Pack::ParameterInfo FrameLib_Pack::sParamInfo;

FrameLib_Pack::FrameLib_Pack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner): FrameLib_MultiChannel(kProcessor, context, owner), mParameters(&sParamInfo)
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
    mOutputs[0].mConnections.clear();
    
    for (unsigned long i = 0; i < getNumIns(); i++)
        for (unsigned long j = 0; j < getInputNumChans(i); j++)
            mOutputs[0].mConnections.push_back(getInputChan(i, j));
    
    return true;
}

// ************************************************************************************** //

// FrameLib_Unpack - Unpack Multichannel Signals

FrameLib_Unpack::ParameterInfo FrameLib_Unpack::sParamInfo;

FrameLib_Unpack::FrameLib_Unpack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_MultiChannel(kProcessor, context, owner), mParameters(&sParamInfo)
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
        mOutputs[i].mConnections.clear();
    
    for (unsigned long i = 0; i < getInputNumChans(0) && i < getNumOuts(); i++)
        mOutputs[i].mConnections.push_back(getInputChan(0, i));
    
    return true;
}
