
#include "FrameLib_Subframe.h"

// Constructor

FrameLib_Subframe::FrameLib_Subframe(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
{
    mParameters.addDouble(kStart, "start", 0.0, 0);
    mParameters.setMin(0.0);
    
    mParameters.addDouble(kEnd, "end", 0.0, 1);
    mParameters.setMin(0.0);
    
    mParameters.addEnum(kUnits, "units", 2);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kRatio, "ratios");
    
    mParameters.setInfo(&sParamInfo);

    mParameters.set(serialisedParameters);
}

// Info

const char *FrameLib_Subframe::objectInfo(bool verbose)
{
    return getInfo("Output part of an input frame: The subframe is specified by a start and end point in the input frame.",
                   "Output part of an input frame.", verbose);
}

const char *FrameLib_Subframe::inputInfo(unsigned long idx, bool verbose)
{
    return "Input Frames";
}

const char *FrameLib_Subframe::outputInfo(unsigned long idx, bool verbose)
{
    return "Subframe Output";
}

// Parameter Info

FrameLib_Subframe::ParameterInfo FrameLib_Subframe::sParamInfo;

FrameLib_Subframe::ParameterInfo::ParameterInfo()
{
    add("Sets the start point of the subframe.");
    add("Sets the end point of the subframe.");
    add("Sets units for the start and end points (samples or ratios [0-1]).");
}

// Process

void FrameLib_Subframe::process()
{
    // Get Input
    
    unsigned long sizeIn, sizeOut;
    double *input = getInput(0, &sizeIn);
    
    unsigned long start, end;
    
    Units units = (Units) mParameters.getInt(kUnits);
    
    if (units == kSamples)
    {
        start = mParameters.getInt(kStart);
        end = mParameters.getInt(kEnd);
    }
    else
    {
        start = round(mParameters.getValue(kStart) * sizeIn);
        end = round(mParameters.getValue(kEnd) * sizeIn);
    }
    
    start = start > sizeIn ? sizeIn : start;
    end = end > sizeIn ? sizeIn : end;
    end = end < start ? start : end;
    
    requestOutputSize(0, end - start);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    // Copy to output
    
    copyVector(output, input + start, sizeOut);
}
