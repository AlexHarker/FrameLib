
#include "FrameLib_Multistream.h"

// FrameLib_Multistream

// Query Connections for Individual Channels

unsigned long FrameLib_Multistream::getInputNumChans(unsigned long inIdx)
{
    MultistreamConnection connection = getConnection(inIdx);
    
    return connection.mObject ? connection.mObject->mOutputs[connection.mIndex].size() : 0;
}

FrameLib_Multistream::BlockConnection FrameLib_Multistream::getInputChan(unsigned long inIdx, unsigned long chan)
{
    MultistreamConnection connection = getConnection(inIdx);

    return connection.mObject->mOutputs[connection.mIndex][chan];
}

unsigned long FrameLib_Multistream::getOrderingConnectionNumChans(unsigned long idx)
{
    MultistreamConnection connection = getOrderingConnection(idx);

    return connection.mObject ? connection.mObject->mOutputs[connection.mIndex].size() : 0;
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

// ************************************************************************************** //

// FrameLib_Pack - Pack Multi-stream signals

FrameLib_Pack::ParameterInfo FrameLib_Pack::sParamInfo;

FrameLib_Pack::FrameLib_Pack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams): FrameLib_Multistream(kProcessor, context, proxy, 1), mParameters(context, proxy, &sParamInfo)
{
    mParameters.addInt(0, "inputs", 2, 0 );
    mParameters.setInstantiation();
    mParameters.set(serialisedParameters);
    setIO(mParameters.getValue(kInputs), 1);
    
    mSerialisedParameters.write(serialisedParameters);
}

std::string FrameLib_Pack::objectInfo(bool verbose)
{
    return formatInfo("Packs multiple frame streams into a multi-stream output: Inputs may be single-stream or multi-stream. All inputs are concatenated in order, with blank inputs ignored.",
                   "Packs multiple frame streams into a multi-stream output.", verbose);
}

std::string FrameLib_Pack::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input # - may be single-stream or multi-stream", "Input #", idx, verbose);
}

std::string FrameLib_Pack::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output - packed multi-stream connection consisting of all input streams", "Multi-stream Output", idx, verbose);
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

// FrameLib_Unpack - Unpack Multi-stream Signals

FrameLib_Unpack::ParameterInfo FrameLib_Unpack::sParamInfo;

FrameLib_Unpack::FrameLib_Unpack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams) : FrameLib_Multistream(kProcessor, context, proxy, 1), mParameters(context, proxy, &sParamInfo)
{
    mParameters.addInt(kOutputs, "outputs", 2, 0);
    mParameters.setInstantiation();
    mParameters.set(serialisedParameters);
    setIO(1, mParameters.getValue(kOutputs));
    
    mSerialisedParameters.write(serialisedParameters);
}

std::string FrameLib_Unpack::objectInfo(bool verbose)
{
    return formatInfo("Unpacks multi-stream input into individual streams: Multi-stream inputs are unpacked in order across the outputs.",
                   "Unpacks multi-stream input into individual streams.", verbose);
}

std::string FrameLib_Unpack::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Multi-stream Input - to be unpacked into individual streams", "Multi-stream Input", idx, verbose);
}

std::string FrameLib_Unpack::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output # - single-stream", "Output #", idx, verbose);
}

bool FrameLib_Unpack::inputUpdate()
{
    for (unsigned long i = 0; i < getNumOuts(); i++)
        mOutputs[i].clear();
    
    for (unsigned long i = 0; i < getInputNumChans(0) && i < getNumOuts(); i++)
        mOutputs[i].push_back(getInputChan(0, i));
    
    return true;
}
