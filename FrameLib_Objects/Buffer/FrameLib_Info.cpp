
#include "FrameLib_Info.h"

// Constructor

FrameLib_Info::FrameLib_Info(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1), mProxy(dynamic_cast<Proxy *>(proxy)->clone())
{
    mParameters.addString(kBuffer, "buffer", 0);
    
    mParameters.addEnum(kUnits, "units");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.addEnumItem(kSamples, "samples");
    
    mParameters.set(serialisedParameters);
    
    mUnits = (Units) mParameters.getInt(kUnits);
    
    setParameterInput(1);
    setIO(1, 3);
    
    assert(false == 0 && "False does not equal zero");
    
    if (mProxy)
        mProxy->update(mParameters.getString(kBuffer));
}

// Info

std::string FrameLib_Info::objectInfo(bool verbose)
{
    return formatInfo("Outputs the sample rate, number of channels and size of a buffer (in milliseconds). The size units can be changed.",
                      "Outputs the sample rate, channels and size of a buffer.", verbose);
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
    switch (idx)
    {
        case 0: return "Buffer Size";
        case 1: return "Sample Rate";
        case 2: return "Channels";
    }
    
    return "";
} 

// Parameter Info

FrameLib_Info::ParameterInfo FrameLib_Info::sParamInfo;

FrameLib_Info::ParameterInfo::ParameterInfo()
{
    add("Sets the buffer name to use.");
    add("Sets the units of output for the size of the buffer.");
}

// Update

void FrameLib_Info::update()
{
    if (mProxy)
        mProxy->update(mParameters.getString(kBuffer));
    
    mUnits = (Units) mParameters.getInt(kUnits);
}

// Process

void FrameLib_Info::process()
{
    // declare vars
    double buf_samplingRate = 0.0;
    double conversionFactor = 1.0;
    unsigned long length    = 0;
    unsigned long chans     = 0;
    unsigned long size      = 1; // output size is 1 - 1 value per outlet
    
    // allocate inputs and outputs
    
    requestOutputSize(0, size);
    requestOutputSize(1, size);
    requestOutputSize(2, size);
    allocateOutputs();
    
    
    double *output_0 = getOutput(0, &size);
    double *output_1 = getOutput(1, &size);
    double *output_2 = getOutput(2, &size);
    
    // Get buffer
    if (mProxy)
        mProxy->acquire(length, buf_samplingRate, chans);
    
    switch (mUnits)
    {
        case kMS:           conversionFactor = mSamplingRate / 1000;        break;
        case kSeconds:      conversionFactor = mSamplingRate;               break;
        case kSamples:      conversionFactor = 1.0;                         break;
    }
    
    
    if (length != 0 && size != 0)
    {
        output_0[0] = length / conversionFactor;
        output_1[0] = buf_samplingRate;
        output_2[0] = chans;
    }
    
    else // if not empty buffer produce results
    {
        // Zero output if no buffer or memory
        zeroVector(output_0, size);
        zeroVector(output_1, size);
        zeroVector(output_2, size);
        
    }
    if (mProxy)
        mProxy->release();
}

