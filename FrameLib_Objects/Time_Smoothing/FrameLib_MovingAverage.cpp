
#include "FrameLib_MovingAverage.h"
#include "../../FrameLib_Dependencies/Interpolation.hpp"

// Constructor

FrameLib_MovingAverage::FrameLib_MovingAverage(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 5, 2)
, mFrameSize(0)
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

// Info

std::string FrameLib_MovingAverage::objectInfo(bool verbose)
{
    return formatInfo("Calculates per sample moving averages and standard deviations: "
                      "The moving average and standard deviations are exponentially weighted. "
                      "An alpha value [0-1] controls the rate of update from static to immediate. "
                      "Alphas may be set per sample using the corresponding input or by parameters. "
                      "Parameters allow different alphas when the average increases or decreases. "
                      "Frames are expected to be of a uniform length. "
                      "The output is the same length as the input. "
                      "If the input length changes then the average and standard deviations are reset. "
                      "These can also be individually reset using the corresponding reset inputs. "
                      "Frames at the reset inputs set the starting values for calculation. "
                      "When these are too short they are padded with values provided by parameter.",
                      "Calculates per sample moving averages and standard deviations.", verbose);
}

std::string FrameLib_MovingAverage::inputInfo(unsigned long idx, bool verbose)
{
   switch (idx)
    {
        case 0:     return "Input";
        case 1:     return "Alphas";
        case 2:     return formatInfo("Average Reset - resets and sets values", "Average Reset", verbose);
        case 3:     return formatInfo("Deviation Reset - resets and sets values", "Deviation Reset", verbose);
        default:    return parameterInputInfo(verbose);
    }
}

std::string FrameLib_MovingAverage::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return "Standard Deviations";
    else
        return "Averages";
}

// Parameter Info

FrameLib_MovingAverage::ParameterInfo FrameLib_MovingAverage::sParamInfo;

FrameLib_MovingAverage::ParameterInfo::ParameterInfo()
{
    add("Sets the alpha value when the average is increasing. "
        "This value is only used if a sufficiently long alpha frame is not present.");
    add("Sets the alpha value when the average is increasing. "
        "This value is only used if a sufficiently long alpha frame is not present.");
    add("Sets the padding value for averages. "
        "This value is only used if a sufficiently long average reset frame is not present.");
    add("Sets the padding value for deviations. "
        "This value is only used if a sufficiently long deviation reset frame is not present.");
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
        mAverageFrame = allocAutoArray<double>(sizeIn);
        mVarianceFrame = allocAutoArray<double>(sizeIn);
        mFrameSize = mAverageFrame && mVarianceFrame ? sizeIn : 0;
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
        
        mLastAvgResetTime = getInputFrameTime(2);
    }
    
    if (resetDeviation)
    {
        const double *devDefaults = getInput(3, &sizeIn);
        PaddedVector deviations(devDefaults, sizeIn, mParameters.getValue(kDeviation));
        
        for (unsigned long i = 0; i < mFrameSize; i++)
        {
            const double deviation = deviations[i];
            mVarianceFrame[i] = deviation * deviation;
        }
        
        mLastDevResetTime = getInputFrameTime(3);
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

