
#include "FrameLib_TimeMedian.h"

FrameLib_TimeMedian::FrameLib_TimeMedian(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_TimeBuffer<FrameLib_TimeMedian>(context, serialisedParameters, owner), mOrdered(NULL), mNumFrames(0)
{}

// Info

std::string FrameLib_TimeMedian::objectInfo(bool verbose)
{
    return formatInfo("Outputs the current time: Time is reported in the specified units. Output is a single value.",
                   "Outputs the current time.", verbose);
}

std::string FrameLib_TimeMedian::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - input frames generate output", "Trigger Input", verbose);
}

std::string FrameLib_TimeMedian::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_TimeMedian::ParameterInfo FrameLib_TimeMedian::sParamInfo;

FrameLib_TimeMedian::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used to for output.");
}

// Update size

void FrameLib_TimeMedian::resetSize(unsigned long maxFrames, unsigned long size)
{
    dealloc(mOrdered);
    mOrdered = alloc<double>(size * maxFrames);
    zeroVector(mOrdered, size * maxFrames);
    mNumFrames = 0;
}

unsigned long find(double input, double *channel, unsigned long numFrames)
{
    unsigned long gap = numFrames >> 1;
    unsigned long i = gap;
    
    gap = gap < 1 ? 1 : gap;
    
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

void FrameLib_TimeMedian::exchange(const double *newFrame, const double *oldFrame, unsigned long size)
{
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

bool checkArray(const double *array, unsigned long size)
{
    for (unsigned long i = 1; i < size; i++)
        if (array[i] < array[i - 1])
            return false;
            
    return true;
}

void FrameLib_TimeMedian::add(const double *newFrame, unsigned long size)
{
    assert(mNumFrames < getMaxFrames() && "Number of frames cannot be increased above the maximum");
    
    for (unsigned long i = 0; i < size; i++)
    {
        // Find insertion point
        
        double *channel = getChannel(i);
        unsigned long j = find(newFrame[i], channel, mNumFrames);
        
        // Copy
        
        std::copy_backward(channel + j, channel + mNumFrames, channel + mNumFrames + 1);
        channel[j] = newFrame[i];
        
        //assert(checkArray(channel, mNumFrames + 1) && "Array out of order");
    }
    
    mNumFrames++;
}

void FrameLib_TimeMedian::remove(const double *oldFrame, unsigned long size)
{
    assert(mNumFrames && "Number of frames cannot be reduced to zero!");
    
    for (unsigned long i = 0; i < size; i++)
    {
        // Find removal point
        
        double *channel = getChannel(i);
        unsigned long j = find(oldFrame[i], channel, mNumFrames);
        
        assert(j < mNumFrames && "value out of place");

        // Copy
       
        std::copy(channel + j + 1, channel + mNumFrames, channel + j);
        
        assert(checkArray(channel, mNumFrames - 1) && "Array out of order");
    }
    
    mNumFrames--;
}

void FrameLib_TimeMedian::result(double *output, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        output[i] = getChannel(i)[mNumFrames >> 1];
}
