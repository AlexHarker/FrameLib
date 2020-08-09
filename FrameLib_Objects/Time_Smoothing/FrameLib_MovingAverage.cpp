
#include "FrameLib_MovingAverage.h"
#include "../../FrameLib_Dependencies/Interpolation.hpp"

// Constructor / Destructor

FrameLib_MovingAverage::FrameLib_MovingAverage(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 2), mAverageFrame(nullptr), mVarianceFrame(nullptr), mFrameSize(0)
{
    mParameters.addDouble(kAlphaUp, "alpha_up", 0.5, 0);
    mParameters.setClip(0.0, 1.0);
    mParameters.addDouble(kAlphaDown, "alpha_down", 0.5, 1);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.set(serialisedParameters);

    setParameterInput(1);
}

FrameLib_MovingAverage::~FrameLib_MovingAverage()
{
    dealloc(mAverageFrame);
    dealloc(mVarianceFrame);
}

// Info

std::string FrameLib_MovingAverage::objectInfo(bool verbose)
{
    //return formatInfo("Calculates a per sample exponentially weighted moving average: Averaging takes a parameter alpha to determine the rate of change. This parameter can be set differently for increasing and decreasing values. Frames are expected to be of uniform size, otherwise the object will reset. The output is the same size as the input.",
    //"Calculates an exponentially weighted moving average.", verbose);
    return formatInfo("Calculates a per sample exponentially weighted moving standard deviation: Averaging takes a parameter alpha to determine the rate of change. This parameter can be set differently for increasing and decreasing values. Frames are expected to be of uniform size, otherwise the object will reset. The output is the same size as the input.",
                   "Calculates an exponentially weighted moving standard deviation.", verbose);
}

std::string FrameLib_MovingAverage::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Frames to Calculate", "Frames to Calculate", verbose);
}

std::string FrameLib_MovingAverage::outputInfo(unsigned long idx, bool verbose)
{
    return idx ? "Standard Deviations" : "Averages";
}

// Parameter Info

FrameLib_MovingAverage::ParameterInfo FrameLib_MovingAverage::sParamInfo;

FrameLib_MovingAverage::ParameterInfo::ParameterInfo()
{
    add("Sets the alpha value for weighting when the input is increasing.");
    add("Sets the alpha value for weighting when the input is decreasing.");
}

// Object Reset

void FrameLib_MovingAverage::objectReset()
{
    mFrameSize = 0;
    mLastResetTime = FrameLib_TimeFormat(0);
}

// Process

void FrameLib_MovingAverage::process()
{
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    double alphaUp = mParameters.getValue(kAlphaUp);
    double alphaDown = mParameters.getValue(kAlphaDown);
    
    // FIX - reset behaviour (reset to zero, default to value, use input)?
    // FIX - how to decide on alphas
    // FIX - option for per frame alphas?
    
    if (mFrameSize != sizeIn)
    {
        dealloc(mAverageFrame);
        dealloc(mVarianceFrame);
        mAverageFrame = alloc<double>(sizeIn);
        mVarianceFrame = alloc<double>(sizeIn);
        mFrameSize = mAverageFrame && mVarianceFrame ? sizeIn : 0;
        zeroVector(mAverageFrame, mFrameSize);
        zeroVector(mVarianceFrame, mFrameSize);
    }
    
    requestOutputSize(0, mFrameSize);
    requestOutputSize(1, mFrameSize);
    allocateOutputs();
    
    double *avgOutput = getOutput(0, &sizeOut);
    double *stdOutput = getOutput(1, &sizeOut);

    for (unsigned long i = 0; i < sizeOut; i++)
    {
        double current = input[i];
        double alpha = current > mAverageFrame[i] ? alphaUp : alphaDown;
       
        double diff = current - mAverageFrame[i];
        double incr = alpha * diff;
        mAverageFrame[i] += incr;
        mVarianceFrame[i] = (1.0 - alpha) * (mVarianceFrame[i] + diff * incr);

        avgOutput[i] = mAverageFrame[i];
        stdOutput[i] = sqrt(mVarianceFrame[i]);
    }
}
