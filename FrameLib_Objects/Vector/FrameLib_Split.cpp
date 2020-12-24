
#include "FrameLib_Split.h"

// Constructor

FrameLib_Split::FrameLib_Split(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 2)
{
    mParameters.addDouble(kSplit, "split", 0.0, 0);
    mParameters.setMin(0.0);
    
    mParameters.addEnum(kUnits, "units", 1);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kRatio, "ratios");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Split::objectInfo(bool verbose)
{
    return formatInfo("Splits input frames into two frames, given a specified split point: "
                      "The split point may be specified in samples or as a ratio of the length of the input frame.",
                      "Splits input frames into two frames, given a specified split point.", verbose);
}

std::string FrameLib_Split::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_Split::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output #", idx);
}

// Parameter Info

FrameLib_Split::ParameterInfo FrameLib_Split::sParamInfo;

FrameLib_Split::ParameterInfo::ParameterInfo()
{
    add("Sets the split point.");
    add("Sets the units for the split point (samples or ratios [0-1]).");
}

// Process

void FrameLib_Split::process()
{
    // Get Input
    
    unsigned long sizeIn, sizeOut1, sizeOut2, split;
    const double *input = getInput(0, &sizeIn);
    Units units = mParameters.getEnum<Units>(kUnits);
    
    // Calculate split point
    
    if (units == kSamples)
        split = mParameters.getInt(kSplit);
    else
        split = roundToUInt(mParameters.getValue(kSplit) * sizeIn);
    
    split = std::min(split, sizeIn);
    
    requestOutputSize(0, split);
    requestOutputSize(1, sizeIn - split);
    allocateOutputs();
    
    double *output1 = getOutput(0, &sizeOut1);
    double *output2 = getOutput(1, &sizeOut2);
    
    // Copy to outputs
    
    copyVector(output1, input, sizeOut1);
    copyVector(output2, input + split, sizeOut2);
}
