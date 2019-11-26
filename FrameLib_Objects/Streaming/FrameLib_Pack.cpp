
#include "FrameLib_Pack.h"

// Constructor

FrameLib_Pack::FrameLib_Pack(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams): FrameLib_Multistream(kProcessor, context, proxy, 1), mParameters(context, proxy, &sParamInfo)
{
    mParameters.addInt(0, "inputs", 2, 0 );
    mParameters.setInstantiation();
    mParameters.set(serialisedParameters);
    setIO(mParameters.getInt(kInputs), 1);
    
    mSerialisedParameters.write(serialisedParameters);
}

// Info

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

// Parameter Info

FrameLib_Pack::ParameterInfo FrameLib_Pack::sParamInfo;

