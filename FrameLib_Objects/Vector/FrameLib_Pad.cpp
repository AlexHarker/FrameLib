
#include "FrameLib_Pad.h"

// Constructor

FrameLib_Pad::FrameLib_Pad(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 2, 1)
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
    return formatInfo("Pads an input frame with a fixed value at either the start the end, or both: "
                   "The output frame size is at least the same length as the input, plus the number of padding values. "
                   "Padding amounts may be set in samples, or as a ratio of the input length.",
                   "Pads an input frame with a fixed value at either the start the end, or both.", verbose);
}

std::string FrameLib_Pad::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Frames to Pad";
}

std::string FrameLib_Pad::outputInfo(unsigned long idx, bool verbose)
{
    return "Padded Frames";
}

// Parameter Info

FrameLib_Pad::ParameterInfo FrameLib_Pad::sParamInfo;

FrameLib_Pad::ParameterInfo::ParameterInfo()
{
    add("Sets the padding value.");
    add("Sets the amount of padding to apply at the start of the frame.");
    add("Sets the amount of padding to apply at the end of the frame.");
    add("Sets units for padding amounts (samples or ratios [0-1]).");
}

// Process

void FrameLib_Pad::process()
{
    // Get Input
    
    unsigned long sizeIn, sizeOut, padStart, padEnd;
    const double *input = getInput(0, &sizeIn);
    double padValue = mParameters.getValue(kPadding);
    Units units = (Units) mParameters.getInt(kUnits);
    
    // Calculate pad amounts
    
    if (units == kSamples)
    {
        padStart = mParameters.getInt(kStart);
        padEnd = mParameters.getInt(kEnd);
    }
    else
    {
        padStart = round(mParameters.getValue(kStart) * sizeIn);
        padEnd = round(mParameters.getValue(kEnd) * sizeIn);
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
