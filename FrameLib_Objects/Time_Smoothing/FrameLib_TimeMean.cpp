
#include "FrameLib_TimeMean.h"

FrameLib_TimeMean::FrameLib_TimeMean(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_TimeBuffer<FrameLib_TimeMean>(context, serialisedParameters, proxy), mSum(nullptr), mCompensate(nullptr)
{}

// Info

std::string FrameLib_TimeMean::objectInfo(bool verbose)
{
    return formatInfo("Outputs the mean per sample over a given number of frames: Frames are expected to be of uniform size, otherwise the buffer is reset. The number of frames (as well as the maximum number of frames) can be set as parameters. The output is the same size as the input.",
                   "Outputs the mean per sample over a given number of frames.", verbose);
}

std::string FrameLib_TimeMean::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Input Values", "Input Values", verbose);
}

std::string FrameLib_TimeMean::outputInfo(unsigned long idx, bool verbose)
{
    return "Means Over Time";
}

// Update size

void FrameLib_TimeMean::resetSize(unsigned long maxFrames, unsigned long size)
{
    dealloc(mSum);
    dealloc(mCompensate);
    mSum = alloc<double>(size);
    mCompensate = alloc<double>(size);
    zeroVector(mSum, size);
    zeroVector(mCompensate, size);
}

// High Precision Sum

static void neumaierSum(double in, double &sum, double &c)
{
    double t = sum + in;
    
    if (fabs(sum) >= fabs(in))
        c += (sum - t) + in;
    else
        c += (in - t) + sum;
    
    sum = t;
}

// Process

void FrameLib_TimeMean::add(const double *newFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        neumaierSum(newFrame[i], mSum[i], mCompensate[i]);
}

void FrameLib_TimeMean::remove(const double *oldFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        neumaierSum(-oldFrame[i], mSum[i], mCompensate[i]);
}

void FrameLib_TimeMean::result(double *output, unsigned long size)
{
    double recip = 1.0 / getNumFrames();
    
    for (unsigned long i = 0; i < size; i++)
        output[i] = (mSum[i] + mCompensate[i]) * recip;
}
