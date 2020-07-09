
#include "FrameLib_Pack.h"

// Constructor

FrameLib_Pack::FrameLib_Pack(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams)
: FrameLib_Multistream(kProcessor, context, proxy, false, 1)
, mParameters(context, proxy, &sParamInfo)
{    
    mParameters.addInt(kInputs, "num_ins", 2, 0 );
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    setIO(mParameters.getInt(kInputs), 1);
    
    mSerialisedParameters.write(serialisedParameters);
}

// Info

std::string FrameLib_Pack::objectInfo(bool verbose)
{
    return formatInfo("Packs multiple frame streams into a multistream output: "
                      "Inputs may be single stream or multistream. "
                      "All inputs are concatenated in order, with blank inputs ignored.",
                      "Packs multiple frame streams into a multistream output.", verbose);
}

std::string FrameLib_Pack::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input # - may be single stream or multistream", "Input #", idx, verbose);
}

std::string FrameLib_Pack::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output - packed multistream connection consisting of all input streams", "Multistream Output", idx, verbose);
}

bool FrameLib_Pack::inputUpdate()
{
    unsigned long size = 0;
    bool change = false;

    // Calculate output size and check for a change of size
    
    for (unsigned long i = 0; i < getNumIns(); i++)
        size += getInputNumStreams(i);
    
    if (size != mOutputs[0].size())
    {
        mOutputs[0].resize(size);
        change = true;
    }
    
    // As we assign compare every connection in turn to check for changes
        
    for (unsigned long i = 0, k = 0; i < getNumIns(); i++)
    {
        for (unsigned long j = 0; j < getInputNumStreams(i); j++, k++)
        {
            change |= getInputChan(i, j) != mOutputs[0][k];
            mOutputs[0][k] = getInputChan(i, j);
        }
    }
    
    return change;
}

// Parameter Info

FrameLib_Pack::ParameterInfo FrameLib_Pack::sParamInfo;

FrameLib_Pack::ParameterInfo::ParameterInfo()
{
    add("Sets the number of inputs."); 
}
