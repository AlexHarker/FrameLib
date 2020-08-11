
#include "FrameLib_MovingAverage.h"
#include "../../FrameLib_Dependencies/Interpolation.hpp"

// Constructor / Destructor

FrameLib_MovingAverage::FrameLib_MovingAverage(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 5, 2), mAverageFrame(nullptr), mVarianceFrame(nullptr), mFrameSize(0)
{
    mParameters.addDouble(kAlphaUp, "alpha_up", 0.5, 0);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.addDouble(kAlphaDown, "alpha_down", 0.5, 1);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.addDouble(kAverage, "average", 0.0, 2);
    
    mParameters.addDouble(kDeviation, "deviation", 0.0, 3);
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(4);
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
    switch (idx)
    {
        case 0:     return "Input";
        case 1:     return "Alphas";
        case 2:     return "Average Reset Input";
        case 3:     return "Deviation Reset Input";
        default:    return parameterInputInfo(verbose);
    }
}

std::string FrameLib_MovingAverage::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return "Deviations";
    else
        return "Averages";
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
    mLastAvgResetTime = FrameLib_TimeFormat(0);
    mLastDevResetTime = FrameLib_TimeFormat(0);
}

double getAlpha(const double *alphas, unsigned long size, unsigned long idx, double defaultValue)
{
    return size ? alphas[std::min(idx, size - 1)] : defaultValue;
}

// Process

void FrameLib_MovingAverage::process()
{
    unsigned long sizeIn, sizeOut, sizeAlphas;
    const double *input = getInput(0, &sizeIn);
    const double *alphas = getInput(1, &sizeAlphas);
    double alphaUp = mParameters.getValue(kAlphaUp);
    double alphaDown = mParameters.getValue(kAlphaDown);
    
    bool resetAverage = mLastAvgResetTime != getInputFrameTime(2);
    bool resetDeviation = mLastDevResetTime != getInputFrameTime(3);
    
    if (mFrameSize != sizeIn)
    {
        dealloc(mAverageFrame);
        dealloc(mVarianceFrame);
        mAverageFrame = alloc<double>(sizeIn);
        mVarianceFrame = alloc<double>(sizeIn);
        mFrameSize = mAverageFrame && mVarianceFrame ? sizeIn : 0;
        mLastAvgResetTime = getInputFrameTime(2);
        mLastDevResetTime = getInputFrameTime(3);
        resetAverage = true;
        resetDeviation = true;
    }
    
    requestOutputSize(0, mFrameSize);
    requestOutputSize(1, mFrameSize);
    allocateOutputs();
    
    double *avgOutput = getOutput(0, &sizeOut);
    double *stdOutput = getOutput(1, &sizeOut);
    
    // Resets
    
    if (resetAverage)
    {
        const double *avgDefaults = getInput(2, &sizeIn);
        PaddedVector averages(avgDefaults, sizeIn, mParameters.getValue(kAverage));
        
        for (unsigned long i = 0; i < mFrameSize; i++)
            mAverageFrame[i] = averages[i];
    }
    
    if (resetDeviation)
    {
        const double *devDefaults = getInput(2, &sizeIn);
        PaddedVector deviations(devDefaults, sizeIn, mParameters.getValue(kAverage));
        
        for (unsigned long i = 0; i < mFrameSize; i++)
        {
            const double deviation = deviations[i];
            mVarianceFrame[i] = deviation * deviation;
        }
    }
    
    // Calculate outputs
    
    if (!resetAverage && !resetDeviation)
    {
        for (unsigned long i = 0; i < sizeOut; i++)
        {
            double defaultAlpha = input[i] > mAverageFrame[i] ? alphaUp : alphaDown;
            double alpha = getAlpha(alphas, sizeAlphas, i, defaultAlpha);
            
            double delta = input[i] - mAverageFrame[i];
            mAverageFrame[i] += alpha * delta;
            mVarianceFrame[i] = (1.0 - alpha) * (mVarianceFrame[i] + alpha * delta * delta);
            
            avgOutput[i] = mAverageFrame[i];
            stdOutput[i] = sqrt(mVarianceFrame[i]);
        }
    }
    else if (!resetAverage)
    {
        for (unsigned long i = 0; i < sizeOut; i++)
        {
            double defaultAlpha = input[i] > mAverageFrame[i] ? alphaUp : alphaDown;
            double alpha = getAlpha(alphas, sizeAlphas, i, defaultAlpha);
            
            mAverageFrame[i] = linear_interp<double>()(alpha, mAverageFrame[i], input[i]);
            
            avgOutput[i] = mAverageFrame[i];
            stdOutput[i] = sqrt(mVarianceFrame[i]);
        }
    }
    else if (!resetDeviation)
    {
        for (unsigned long i = 0; i < sizeOut; i++)
        {
            double defaultAlpha = input[i] > mAverageFrame[i] ? alphaUp : alphaDown;
            double alpha = getAlpha(alphas, sizeAlphas, i, defaultAlpha);
            
            double delta = input[i] - mAverageFrame[i];
            mVarianceFrame[i] = (1.0 - alpha) * (mVarianceFrame[i] + alpha * delta * delta);
            
            avgOutput[i] = mAverageFrame[i];
            stdOutput[i] = sqrt(mVarianceFrame[i]);
        }
    }
    else
    {
        for (unsigned long i = 0; i < sizeOut; i++)
        {
            avgOutput[i] = mAverageFrame[i];
            stdOutput[i] = sqrt(mVarianceFrame[i]);
        }
    }
}

