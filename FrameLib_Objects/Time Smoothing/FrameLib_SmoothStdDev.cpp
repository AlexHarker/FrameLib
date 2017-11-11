
#include "FrameLib_SmoothStdDev.h"

FrameLib_SmoothStdDev::FrameLib_SmoothStdDev(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_TimeSmoothing<FrameLib_SmoothStdDev>(context, serialisedParameters, owner), mSum(NULL), mCompensate(NULL), mSqSum(NULL), mSqCompensate(NULL)
{
}

// Info

std::string FrameLib_SmoothStdDev::objectInfo(bool verbose)
{
    return formatInfo("Outputs the current time: Time is reported in the specified units. Output is a single value.",
                   "Outputs the current time.", verbose);
}

std::string FrameLib_SmoothStdDev::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - input frames generate output", "Trigger Input", verbose);
}

std::string FrameLib_SmoothStdDev::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_SmoothStdDev::ParameterInfo FrameLib_SmoothStdDev::sParamInfo;

FrameLib_SmoothStdDev::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used to for output.");
}

// Update size

void FrameLib_SmoothStdDev::resetSize(unsigned long maxFrames, unsigned long size)
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

void neumaierSum(double in, double &sum, double &c)
{
    double t = sum + in;
    
    if (fabs(sum) >= fabs(in))
        c += (sum - t) + in;
    else
        c += (in - t) + sum;
    
    sum = t;
}

// Process

void FrameLib_SmoothStdDev::add(const double *newFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
    {
        neumaierSum(newFrame[i], mSum[i], mCompensate[i]);
        neumaierSum(newFrame[i] * newFrame[i], mSqSum[i], mSqCompensate[i]);
    }
}

void FrameLib_SmoothStdDev::remove(const double *oldFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
    {
        neumaierSum(-oldFrame[i], mSum[i], mCompensate[i]);
        neumaierSum(-(oldFrame[i] * oldFrame[i]), mSqSum[i], mSqCompensate[i]);
    }
}


void FrameLib_SmoothStdDev::result(double *output, unsigned long size)
{
    double recip = 1.0 / getNumFrames();
    
    for (unsigned long i = 0; i < size; i++)
    {
        double sum = mSum[i] + mCompensate[i];
        double sqSum = mSqSum[i] + mSqCompensate[i];
        output[i] = sqrt((sqSum - ((sum * sum) * recip)) * recip);
    }
}
