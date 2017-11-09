
#include "FrameLib_SmoothMean.h"

FrameLib_SmoothMean::FrameLib_SmoothMean(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_TimeSmoothing<FrameLib_SmoothMean>(context, serialisedParameters, owner), mSum(NULL), mCompensate(NULL)
{
}

// Info

std::string FrameLib_SmoothMean::objectInfo(bool verbose)
{
    return formatInfo("Outputs the current time: Time is reported in the specified units. Output is a single value.",
                   "Outputs the current time.", verbose);
}

std::string FrameLib_SmoothMean::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - input frames generate output", "Trigger Input", verbose);
}

std::string FrameLib_SmoothMean::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_SmoothMean::ParameterInfo FrameLib_SmoothMean::sParamInfo;

FrameLib_SmoothMean::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used to for output.");
}

// Update size

void FrameLib_SmoothMean::resetSize(unsigned long size)
{
    dealloc(mSum);
    dealloc(mCompensate);
    mSum = alloc<double>(size);
    mCompensate = alloc<double>(size);
    zeroVector(mSum, size);
    zeroVector(mCompensate, size);
}

// High Precision Sum

double neumaierSum(double in, double &sum, double &c)
{
    double t = sum + in;
    
    if (fabs(sum) >= fabs(in))
        c += (sum - t) + in;
    else
        c += (in - t) + sum;
    
    sum = t;
    
    return sum + c;
}

// Process

void FrameLib_SmoothMean::smooth(double *output, const double *newFrame, const double *oldestFrame, unsigned long size)
{
    double recip = 1.0 / getNumFrames();
    
    for (unsigned long i = 0; i < size; i++)
    {
        neumaierSum(-oldestFrame[i], mSum[i], mCompensate[i]);
        output[i] = neumaierSum(newFrame[i], mSum[i], mCompensate[i]) * recip;
    }
}
