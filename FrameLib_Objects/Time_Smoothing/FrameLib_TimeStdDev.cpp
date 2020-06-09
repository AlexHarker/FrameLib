
#include "FrameLib_TimeStdDev.h"

// Constructor / Destructor

FrameLib_TimeStdDev::FrameLib_TimeStdDev(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_TimeBuffer<FrameLib_TimeStdDev, false>(context, serialisedParameters, proxy), mSum(nullptr), mCompensate(nullptr), mSqSum(nullptr), mSqCompensate(nullptr)
{
}

FrameLib_TimeStdDev::~FrameLib_TimeStdDev()
{
    dealloc(mSum);
    dealloc(mCompensate);
    dealloc(mSqSum);
    dealloc(mSqCompensate);
}

// Info

std::string FrameLib_TimeStdDev::objectInfo(bool verbose)
{
    return formatInfo("Outputs the standard deviation per sample over a given number of frames: Frames are expected to be of uniform size, otherwise the buffer is reset. The number of frames (as well as the maximum number of frames) can be set as parameters. The output is the same size as the input.",
                      "Outputs the standard deviation per sample over a given number of frames.", verbose);
}

std::string FrameLib_TimeStdDev::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Input Values", "Input Values", verbose);
}

std::string FrameLib_TimeStdDev::outputInfo(unsigned long idx, bool verbose)
{
    return "Standard Deviations Over Time";
}

// Update size

void FrameLib_TimeStdDev::resetSize(unsigned long maxFrames, unsigned long size)
{
    dealloc(mSum);
    dealloc(mCompensate);
    dealloc(mSqSum);
    dealloc(mSqCompensate);
    
    mSum = alloc<double>(size);
    mCompensate = alloc<double>(size);
    mSqSum = alloc<double>(size);
    mSqCompensate = alloc<double>(size);
    
    zeroVector(mSum, size);
    zeroVector(mCompensate, size);
    zeroVector(mSqSum, size);
    zeroVector(mSqCompensate, size);
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

void FrameLib_TimeStdDev::add(const double *newFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
    {
        neumaierSum(newFrame[i], mSum[i], mCompensate[i]);
        neumaierSum(newFrame[i] * newFrame[i], mSqSum[i], mSqCompensate[i]);
    }
}

void FrameLib_TimeStdDev::remove(const double *oldFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
    {
        neumaierSum(-oldFrame[i], mSum[i], mCompensate[i]);
        neumaierSum(-(oldFrame[i] * oldFrame[i]), mSqSum[i], mSqCompensate[i]);
    }
}


void FrameLib_TimeStdDev::result(double *output, unsigned long size)
{
    double recip = 1.0 / getNumFrames();
    
    for (unsigned long i = 0; i < size; i++)
    {
        double sum = mSum[i] + mCompensate[i];
        double sqSum = mSqSum[i] + mSqCompensate[i];
        double variance = (sqSum - ((sum * sum) * recip)) * recip;
        
        if (variance < 0.0)
            variance = 0.0;
        
        output[i] = sqrt(variance);        
    }
}
