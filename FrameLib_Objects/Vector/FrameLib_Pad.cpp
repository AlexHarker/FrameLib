
#include "FrameLib_Pad.h"

// Constructor

FrameLib_Pad::FrameLib_Pad(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addDouble(kPadding, "pad", 0.0, 0);
    
    mParameters.addDouble(kStart, "start", 0.0, 1);
    mParameters.setMin(0.0);
    
    mParameters.addDouble(kEnd, "end", 0.0, 2);
    mParameters.setMin(0.0);
    
    mParameters.addEnum(kUnits, "units", 3);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kRatio, "ratios");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Pad::objectInfo(bool verbose)
{
    return formatInfo("Pads the input frame with a repeated specified value: "
                      "Padding may be applied to either the start or the end of the frame or both. "
                      "Padding amounts may be set in samples, or as a ratio of the input length. "
                      "The output length is at least the size of the input.",
                      "Pads the input frame with a repeated specified value.", verbose);
}

std::string FrameLib_Pad::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_Pad::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Pad::ParameterInfo FrameLib_Pad::sParamInfo;

FrameLib_Pad::ParameterInfo::ParameterInfo()
{
    add("Sets the padding value.");
    add("Sets the amount of padding to apply to the start of the frame.");
    add("Sets the amount of padding to apply to the end of the frame.");
    add("Sets the units for padding amounts (samples or ratios [0-1]).");
}

// Process

void FrameLib_Pad::process()
{
    // Get Input
    
    unsigned long sizeIn, sizeOut, padStart, padEnd;
    const double *input = getInput(0, &sizeIn);
    double padValue = mParameters.getValue(kPadding);
    Units units = mParameters.getEnum<Units>(kUnits);
    
    // Calculate pad amounts
    
    if (units == kSamples)
    {
        padStart = mParameters.getInt(kStart);
        padEnd = mParameters.getInt(kEnd);
    }
    else
    {
        padStart = roundToUInt(mParameters.getValue(kStart) * sizeIn);
        padEnd = roundToUInt(mParameters.getValue(kEnd) * sizeIn);
    }
    
    requestOutputSize(0, padStart + sizeIn + padEnd);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    // Copy to output with padding
    
    if (sizeOut)
    {
        for (unsigned long i = 0; i < padStart; i++)
            output[i] = padValue;
        
        copyVector(output + padStart, input, sizeIn);
        
        for (unsigned long i = padStart + sizeIn; i < (padStart + sizeIn + padEnd); i++)
            output[i] = padValue;
    }
}
