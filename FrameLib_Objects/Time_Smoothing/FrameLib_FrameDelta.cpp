
#include "FrameLib_FrameDelta.h"

// Constructor

FrameLib_FrameDelta::FrameLib_FrameDelta(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, nullptr, 1, 1)
, mFrameSize(0)
{}

// Info

std::string FrameLib_FrameDelta::objectInfo(bool verbose)
{
    return formatInfo("Outputs the deltas between two consecutive frames: "
                      "Frames are expected to be of uniform size, otherwise the object will reset. "
                      "The first frame (after a reset) will consist of all zero values. "
                      "The output is the same size as the input.",
                      "Outputs the deltas between two consecutive frames.", verbose);
}

std::string FrameLib_FrameDelta::inputInfo(unsigned long idx, bool verbose)
{
    return "Input";
}

std::string FrameLib_FrameDelta::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Process

void FrameLib_FrameDelta::process()
{
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    bool reset = false;
    
    if (mFrameSize != sizeIn)
    {
        mLastFrame = allocAutoArray<double>(sizeIn);
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
