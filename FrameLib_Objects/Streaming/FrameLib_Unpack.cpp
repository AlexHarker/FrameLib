
#include "FrameLib_Unpack.h"

// Constructor

FrameLib_Unpack::FrameLib_Unpack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams) : FrameLib_Multistream(kProcessor, context, proxy, 1), mParameters(context, proxy, &sParamInfo)
{
    mParameters.addInt(kOutputs, "outputs", 2, 0);
    mParameters.setInstantiation();
    mParameters.set(serialisedParameters);
    setIO(1, mParameters.getValue(kOutputs));
    
    mSerialisedParameters.write(serialisedParameters);
}

// Info

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

// Parameter Info

FrameLib_Unpack::ParameterInfo FrameLib_Unpack::sParamInfo;






