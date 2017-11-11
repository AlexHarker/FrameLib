
#include "FrameLib_FrameDelta.h"

FrameLib_FrameDelta::FrameLib_FrameDelta(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 1, 1), mLastFrame(NULL), mFrameSize(0)
{
}

// Info

std::string FrameLib_FrameDelta::objectInfo(bool verbose)
{
    return formatInfo("Outputs the current time: Time is reported in the specified units. Output is a single value.",
                   "Outputs the current time.", verbose);
}

std::string FrameLib_FrameDelta::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - input frames generate output", "Trigger Input", verbose);
}

std::string FrameLib_FrameDelta::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_FrameDelta::ParameterInfo FrameLib_FrameDelta::sParamInfo;

FrameLib_FrameDelta::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used to for output.");
}

// Process

void FrameLib_FrameDelta::process()
{
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    bool reset = false;
    
    if (mFrameSize != sizeIn)
    {
        dealloc(mLastFrame);
        mLastFrame = alloc<double>(sizeIn);
        mFrameSize = mLastFrame ? sizeIn : 0;
        reset = true;
    }
    
    requestOutputSize(0, mFrameSize);
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
    
    if (reset)
        zeroVector(output, sizeOut);
    else
        for (unsigned long i = 0; i < sizeOut; i++)
            output[i] = input[i] - mLastFrame[i];
    
    copyVector(mLastFrame, input, mFrameSize);
}
