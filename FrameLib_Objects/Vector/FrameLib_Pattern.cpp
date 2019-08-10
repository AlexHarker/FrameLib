
#include "FrameLib_Pattern.h"

// Constructor

FrameLib_Pattern::FrameLib_Pattern(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1), mPosition(0)
{
    mParameters.addEnum(kMode, "mode");
    mParameters.addEnumItem(kOnce, "once");
    mParameters.addEnumItem(kLoop, "loop");
    
    setInputMode(1, false, false, false);
    addParameterInput();
    
    mParameters.set(serialisedParameters);
}

// Info

std::string FrameLib_Pattern::objectInfo(bool verbose)
{
    return formatInfo("Splits a frame sequentially into smaller frames based on the trigger input.", verbose);
}

std::string FrameLib_Pattern::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return "Trigger Input";
        case 1: return "Pattern Input";
        default:
            return parameterInputInfo(verbose);
    }
}

std::string FrameLib_Pattern::outputInfo(unsigned long idx, bool verbose)
{
    return "Current Frame From Pattern";
}

// Parameter Info

FrameLib_Pattern::ParameterInfo FrameLib_Pattern::sParamInfo;

FrameLib_Pattern::ParameterInfo::ParameterInfo()
{
    add("Set the playback mode. once - the pattern is played once only. loop - the pattern loops infinitely." );
}

// Object Reset

void FrameLib_Pattern::objectReset()
{
    mPosition = 0;
}

// Process

void FrameLib_Pattern::process()
{
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(1, &sizeIn);

    sizeOut = mPosition < sizeIn ? 1 : 0;
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();

    if (getCurrentTime() == getInputFrameTime(1))
        mPosition = 0;
    
    double *output = getOutput(0, &sizeOut);

    if (output)
    {
        output[0] = input ? input[mPosition] : 0.0;
        mPosition++;
        
        if (mParameters.getInt(kMode))
            mPosition %= sizeIn;
    }
}
