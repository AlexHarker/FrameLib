
#include "FrameLib_EWMSD.h"
#include "../../FrameLib_Dependencies/Interpolation.hpp"

FrameLib_EWMSD::FrameLib_EWMSD(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1), mAverageFrame(nullptr), mVarianceFrame(nullptr), mPrevFrame(nullptr), mFrameSize(0)
{
    mParameters.addDouble(kAlphaUp, "alpha_up", 0.5, 0);
    mParameters.setClip(0.0, 1.0);
    mParameters.addDouble(kAlphaDown, "alpha_down", 0.5, 0);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.set(serialisedParameters);

    setParameterInput(1);
}

// Info

std::string FrameLib_EWMSD::objectInfo(bool verbose)
{
    return formatInfo("Calculates a per sample exponentially weighted moving standard deviation: Averaging takes a parameter alpha to determine the rate of change. This parameter can be set differently for increasing and decreasing values. Frames are expected to be of uniform size, otherwise the object will reset. The output is the same size as the input.",
                   "Calculates an exponentially weighted moving standard deviation.", verbose);
}

std::string FrameLib_EWMSD::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Frames to Calculate", "Frames to Calculate", verbose);
}

std::string FrameLib_EWMSD::outputInfo(unsigned long idx, bool verbose)
{
    return "Standard Deviation Values";
}

// Parameter Info

FrameLib_EWMSD::ParameterInfo FrameLib_EWMSD::sParamInfo;

FrameLib_EWMSD::ParameterInfo::ParameterInfo()
{
    add("Sets the alpha value for weighting when the input is increasing.");
    add("Sets the alpha value for weighting when the input is decreasing.");
}

// Process

void FrameLib_EWMSD::process()
{
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    double alphaUp = mParameters.getValue(kAlphaUp);
    double alphaDown = mParameters.getValue(kAlphaDown);
    
    if (mFrameSize != sizeIn)
    {
        dealloc(mAverageFrame);
        dealloc(mVarianceFrame);
        dealloc(mPrevFrame);
        mAverageFrame = alloc<double>(sizeIn);
        mVarianceFrame = alloc<double>(sizeIn);
        mPrevFrame = alloc<double>(sizeIn);
        mFrameSize = mAverageFrame && mVarianceFrame && mPrevFrame ? sizeIn : 0;
        zeroVector(mAverageFrame, mFrameSize);
        zeroVector(mVarianceFrame, mFrameSize);
        zeroVector(mPrevFrame, mFrameSize);
    }
    
    requestOutputSize(0, mFrameSize);
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
    
    for (unsigned long i = 0; i < sizeOut; i++)
    {
        double current = input[i];
        double previous = mPrevFrame[i];
        double alpha = current > previous ? alphaUp : alphaDown;
       
        double diff = current - mAverageFrame[i];
        double incr = alpha * diff;
        mAverageFrame[i] += incr;
        double variance = mVarianceFrame[i] = (1.0 - alpha) * (mVarianceFrame[i] + diff * incr);
        
        output[i] = sqrt(variance);
    }
    
    copyVector(mPrevFrame, input, mFrameSize);
}
