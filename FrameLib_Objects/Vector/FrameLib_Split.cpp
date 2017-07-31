
#include "FrameLib_Split.h"

// Constructor

FrameLib_Split::FrameLib_Split(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 2)
{
    mParameters.addDouble(kSplit, "split", 0.0, 0);
    mParameters.setMin(0.0);
    
    mParameters.addEnum(kUnits, "units", 1);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kRatio, "ratios");
    
    mParameters.setInfo(&sParamInfo);

    mParameters.set(serialisedParameters);
}

// Info

const char *FrameLib_Split::objectInfo(bool verbose)
{
    return getInfo("Splits input frames into two parts, given a specified split point: "
                   "The split point may be specified in samples or as a ratio",
                   "Splits input frames into two parts, given a specified split point.", verbose);
}

const char *FrameLib_Split::inputInfo(unsigned long idx, bool verbose)
{
    return "Frame to Split";
}

const char *FrameLib_Split::outputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Output Frame #", "Output Frame #", idx, verbose);
}

// Parameter Info

FrameLib_Split::ParameterInfo FrameLib_Split::sParamInfo;

FrameLib_Split::ParameterInfo::ParameterInfo()
{
    add("Sets the split point.");
    add("Sets units for the split point (samples or ratios [0-1]).");
}

// Process

void FrameLib_Split::process()
{
    // Get Input
    
    unsigned long sizeIn, sizeOut1, sizeOut2, split;
    double *input = getInput(0, &sizeIn);
    Units units = (Units) mParameters.getInt(kUnits);
    
    // Calculate split point
    
    if (units == kSamples)
        split = mParameters.getInt(kSplit);
    else
        split = round(mParameters.getValue(kSplit) * sizeIn);
    
    split = split > sizeIn ? sizeIn : split;
    
    requestOutputSize(0, split);
    requestOutputSize(1, sizeIn - split);
    allocateOutputs();
    
    double *output1 = getOutput(0, &sizeOut1);
    double *output2 = getOutput(1, &sizeOut2);
    
    // Copy to outputs
    
    copyVector(output1, input, sizeOut1);
    copyVector(output2, input + split, sizeOut2);
}
