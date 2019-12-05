
#include "FrameLib_Info.h"

// Constructor

FrameLib_Info::FrameLib_Info(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 3), mProxy(cloneProxy<Proxy>(proxy))
{
    mParameters.addString(kBuffer, "buffer", 0);
    
    mParameters.addEnum(kUnits, "units", 1);
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.addEnumItem(kSamples, "samples");
    
    mParameters.set(serialisedParameters);
    
    mUnits = (Units) mParameters.getInt(kUnits);
    
    setParameterInput(1);
        
    if (mProxy)
        mProxy->update(mParameters.getString(kBuffer));
}

// Info

std::string FrameLib_Info::objectInfo(bool verbose)
{
    return formatInfo("Outputs the sample rate, number of channels and length of a buffer. "
                      "The units for reporting length can be set with the units parameter.",
                      "Outputs the sample rate, channels and size of a buffer.", verbose);
}

std::string FrameLib_Info::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Trigger Input", "Trigger Input", verbose);
}

std::string FrameLib_Info::outputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return "Buffer Length";
        case 1: return "Sample Rate";
        case 2: return "Number of Channels";
    }
    
    return "";
} 

// Parameter Info

FrameLib_Info::ParameterInfo FrameLib_Info::sParamInfo;

FrameLib_Info::ParameterInfo::ParameterInfo()
{
    add("Sets the buffer to use.");
    add("Sets the units for reporting the length of the buffer.");
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
    double bufferSamplingRate = 0.0;
    double conversionFactor = 1.0;
    unsigned long length = 0;
    unsigned long chans = 0;
    unsigned long size = 1;
    
    // Allocate inputs and outputs
    
    requestOutputSize(0, size);
    requestOutputSize(1, size);
    requestOutputSize(2, size);
    allocateOutputs();
    
    double *lengthOutput = getOutput(0, &size);
    double *smplRtOutput = getOutput(1, &size);
    double *nChansOutput = getOutput(2, &size);
    
    // Get buffer
    
    if (mProxy)
        mProxy->acquire(length, bufferSamplingRate, chans);
    
    switch (mUnits)
    {
        case kMS:           conversionFactor = mSamplingRate / 1000;        break;
        case kSeconds:      conversionFactor = mSamplingRate;               break;
        case kSamples:      conversionFactor = 1.0;                         break;
    }
    
    if (length != 0 && size != 0)
    {
        lengthOutput[0] = length / conversionFactor;
        smplRtOutput[0] = bufferSamplingRate;
        nChansOutput[0] = chans;
    }
    else
    {
        // Zero output if no buffer or memory
        
        zeroVector(lengthOutput, size);
        zeroVector(smplRtOutput, size);
        zeroVector(nChansOutput, size);
    }
    
    if (mProxy)
        mProxy->release();
}

