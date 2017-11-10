
#include "FrameLib_Lag.h"

FrameLib_Lag::FrameLib_Lag(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_TimeSmoothing<FrameLib_Lag>(context, serialisedParameters, owner), mFrameCopy(NULL)
{
}

// Info

std::string FrameLib_Lag::objectInfo(bool verbose)
{
    return formatInfo("Outputs the current time: Time is reported in the specified units. Output is a single value.",
                   "Outputs the current time.", verbose);
}

std::string FrameLib_Lag::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - input frames generate output", "Trigger Input", verbose);
}

std::string FrameLib_Lag::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_Lag::ParameterInfo FrameLib_Lag::sParamInfo;

FrameLib_Lag::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used to for output.");
}

// Update size

void FrameLib_Lag::resetSize(unsigned long size)
{
    dealloc(mFrameCopy);
    mFrameCopy = alloc<double>(size);
    zeroVector(mFrameCopy, size);
}

// Process

void FrameLib_Lag::remove(const double *oldFrame, unsigned long size)
{
    copyVector(mFrameCopy, oldFrame, size);
}

void FrameLib_Lag::result(double *output, unsigned long size)
{
    copyVector(output, mFrameCopy, size);
}
