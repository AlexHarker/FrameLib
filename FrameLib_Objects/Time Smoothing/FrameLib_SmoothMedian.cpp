
#include "FrameLib_SmoothMedian.h"

FrameLib_SmoothMedian::FrameLib_SmoothMedian(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_TimeSmoothing<FrameLib_SmoothMedian>(context, serialisedParameters, owner), mOrdered(NULL), mNumFrames(0)
{}

// Info

std::string FrameLib_SmoothMedian::objectInfo(bool verbose)
{
    return formatInfo("Outputs the current time: Time is reported in the specified units. Output is a single value.",
                   "Outputs the current time.", verbose);
}

std::string FrameLib_SmoothMedian::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - input frames generate output", "Trigger Input", verbose);
}

std::string FrameLib_SmoothMedian::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_SmoothMedian::ParameterInfo FrameLib_SmoothMedian::sParamInfo;

FrameLib_SmoothMedian::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used to for output.");
}

// Update size

void FrameLib_SmoothMedian::resetSize(unsigned long size)
{
    dealloc(mOrdered);
    mOrdered = alloc<double>(size * getMaxFrames());
    zeroVector(mOrdered, size * getMaxFrames());
    mNumFrames = getNumFrames();
}

unsigned long find(double input, double *channel, unsigned long numFrames)
{
    unsigned long gap = numFrames >> 1;
    unsigned long i = gap;
    
    while (gap && i < numFrames)
    {
        gap >>= 1;
        gap = gap < 1 ? 1 : gap;

        if (input < channel[i])
        {
            if (gap == 1 && (!i || input > channel[i - 1]))
                gap = 0;
            else
                i -= gap;
        }
        else if (input > channel[i])
            i += gap;
        else
            break;
    }
    
    return i;
}

// Process

void FrameLib_SmoothMedian::exchange(const double *newFrame, const double *oldFrame, unsigned long size)
{
    mNumFrames = getNumFrames();
    
    for (unsigned long i = 0; i < size; i++)
    {
        // Find insertion points
        
        double *channel = getChannel(i);
        unsigned long j = find(oldFrame[i], channel, mNumFrames);
        unsigned long k = find(newFrame[i], channel, mNumFrames);
        
        // Copy
        
        if (j > k)
            std::copy_backward(channel + k, channel + j, channel + j + 1);
        else if (k > j)
        {
            std::copy(channel + j + 1, channel + k, channel + j);
            k--;
        }
        
        channel[k] = newFrame[i];
    }
}

void FrameLib_SmoothMedian::add(const double *newFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
    {
        // Find insertion point
        
        double *channel = getChannel(i);
        unsigned long j = find(newFrame[i], channel, mNumFrames);
        
        // Copy
        
        std::copy_backward(channel + j, channel + mNumFrames, channel + mNumFrames + 1);
        channel[j] = newFrame[i];
    }
    
    mNumFrames++;
}

void FrameLib_SmoothMedian::remove(const double *oldFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
    {
        // Find removal point
        
        double *channel = getChannel(i);
        unsigned long j = find(oldFrame[i], channel, mNumFrames);
        
        // Copy
       
        std::copy(channel + j + 1, channel + mNumFrames, channel + j);
    }
    
    mNumFrames--;
}

void FrameLib_SmoothMedian::result(double *output, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        output[i] = getChannel(i)[mNumFrames >> 1];
}
