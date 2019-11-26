
#include "FrameLib_EWMA.h"
#include "../../FrameLib_Dependencies/Interpolation.hpp"

FrameLib_EWMA::FrameLib_EWMA(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1), mAverageFrame(nullptr), mPrevFrame(nullptr), mFrameSize(0)
{
    mParameters.addDouble(kAlphaUp, "alpha_up", 0.5, 0);
    mParameters.setClip(0.0, 1.0);
    mParameters.addDouble(kAlphaDown, "alpha_down", 0.5, 0);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.set(serialisedParameters);

    setParameterInput(1);
}

// Info

std::string FrameLib_EWMA::objectInfo(bool verbose)
{
    return formatInfo("Calculates a per sample exponentially weighted moving average: Averaging takes a parameter alpha to determine the rate of change. This parameter can be set differently for increasing and decreasing values. Frames are expected to be of uniform size, otherwise the object will reset. The output is the same size as the input.",
                   "Calculates an exponentially weighted moving average.", verbose);
}

std::string FrameLib_EWMA::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Frames to Average", "Frames to Average", verbose);
}

std::string FrameLib_EWMA::outputInfo(unsigned long idx, bool verbose)
{
    return "Average Values";
}

// Parameter Info

FrameLib_EWMA::ParameterInfo FrameLib_EWMA::sParamInfo;

FrameLib_EWMA::ParameterInfo::ParameterInfo()
{
    add("Sets the alpha value for weighting when the input is increasing.");
    add("Sets the alpha value for weighting when the input is decreasing.");
}

// Process

void FrameLib_EWMA::process()
{
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    double alphaUp = mParameters.getValue(kAlphaUp);
    double alphaDown = mParameters.getValue(kAlphaDown);
    
    if (mFrameSize != sizeIn)
    {
        dealloc(mAverageFrame);
        dealloc(mPrevFrame);
        mAverageFrame = alloc<double>(sizeIn);
        mPrevFrame = alloc<double>(sizeIn);
        mFrameSize = mAverageFrame && mPrevFrame ? sizeIn : 0;
        zeroVector(mAverageFrame, mFrameSize);
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
        
        output[i] = mAverageFrame[i] = linear_interp<double>()(alpha, current, mAverageFrame[i]);
    }
    
    copyVector(mPrevFrame, input, mFrameSize);
}
