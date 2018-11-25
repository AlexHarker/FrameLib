
#include "FrameLib_Info.h"

// Constructor

FrameLib_Info::FrameLib_Info(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1), mProxy(dynamic_cast<Proxy *>(proxy))
{
    mParameters.addString(kBuffer, "buffer", 0);
        
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
    
    assert(false == 0 && "False does not equal zero");
    
    if (mProxy)
        mProxy->update(mParameters.getString(kBuffer));
}

// Info

std::string FrameLib_Info::objectInfo(bool verbose)
{
    return formatInfo("Outputs the sample rate, number of channels and size (in samples) of an (i)buffer~.",
                   "Outputs the sample rate, channels and size of an (i)buffer~.", verbose);
}

std::string FrameLib_Info::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Trigger frame", "Trigger frame", verbose);
}

std::string FrameLib_Info::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Frame";
}

// Parameter Info

FrameLib_Info::ParameterInfo FrameLib_Info::sParamInfo;

FrameLib_Info::ParameterInfo::ParameterInfo()
{
    add("Sets the buffer~ name to use.");
}

// Update

void FrameLib_Info::update()
{
    if (mProxy)
        mProxy->update(mParameters.getString(kBuffer));
}

// Process

void FrameLib_Info::process()
{
    // declare vars
    double samplingRate     = 0.0;
    unsigned long length    = 0;
    unsigned long size      = 2;
    
    // allocate inputs and outputs
    
    requestOutputSize(0, size);
    allocateOutputs();
    
    double *output = getOutput(0, &size);
    
    // Get buffer
    if (mProxy)
        mProxy->acquire(length, samplingRate);
    
    if (length != 0)
    {
        output[0] = length;
        output[1] = samplingRate;
        if (mProxy)
            mProxy->release();
    }
    
    else // if not empty buffer produce results
    {
        // Zero output if no buffer or memory
        zeroVector(output, size);
        if (mProxy)
            mProxy->release();
    }

}
