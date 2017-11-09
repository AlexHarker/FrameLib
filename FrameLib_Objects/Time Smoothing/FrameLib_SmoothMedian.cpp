
#include "FrameLib_SmoothMedian.h"

FrameLib_SmoothMedian::FrameLib_SmoothMedian(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_TimeSmoothing<FrameLib_SmoothMedian>(context, serialisedParameters, owner), mOrdered(NULL)
{
}

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
    mOrdered = alloc<double>(size * getNumFrames());
    zeroVector(mOrdered, size * getNumFrames());
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

void FrameLib_SmoothMedian::smooth(double *output, const double *newFrame, const double *oldestFrame, unsigned long size)
{
    unsigned long numFrames = getNumFrames();
    
    for (unsigned long i = 0; i < size; i++)
    {
        // Find insertion points
        
        double *channel = mOrdered + (i * numFrames);
        unsigned long j = find(oldestFrame[i], channel, numFrames);
        unsigned long k = find(newFrame[i], channel, numFrames);
        
        // Copy
        
        if (j > k)
            std::copy_backward(channel + k, channel + j, channel + j + 1);
        else if (k > j)
        {
            std::copy(channel + j + 1, channel + k, channel + j);
            k--;
        }
        
        channel[k] = newFrame[i];

        // Output
        
        output[i] = channel[numFrames >> 1];
    }
}
