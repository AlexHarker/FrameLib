
#include "FrameLib_TimeMedian.h"

// Constructor / Destructor

FrameLib_TimeMedian::FrameLib_TimeMedian(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_TimeBuffer<FrameLib_TimeMedian, 1>(context, serialisedParameters, proxy)
, mOrdered(nullptr)
, mNumFrames(0)
{
    mParameters.addDouble(kPercentile, "percentile", 50.0, kPercentile);
    mParameters.setClip(0.0, 100.0);

    completeDefaultParameters(serialisedParameters);
}

FrameLib_TimeMedian::~FrameLib_TimeMedian()
{
    dealloc(mOrdered);
}

// Info

const char *FrameLib_TimeMedian::paramInfo(unsigned long idx)
{
    return "Sets the percentile to retrieve";
}

std::string FrameLib_TimeMedian::objectInfo(bool verbose)
{
    return formatInfo("Outputs the median per sample over a given number of frames: Frames are expected to be of uniform size, otherwise the buffer is reset. The number of frames (as well as the maximum number of frames) can be set as parameters. The output is the same size as the input.", "Outputs the median per sample over a given number of frames.", verbose);
}

std::string FrameLib_TimeMedian::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:     return "Input";
        case 1:     return "Reset Input";
        default:    return parameterInputInfo(verbose);
    }
}

std::string FrameLib_TimeMedian::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return "Buffer Full";
    else
        return "Output";
}

// Update size

void FrameLib_TimeMedian::resetSize(unsigned long maxFrames, unsigned long size)
{
    dealloc(mOrdered);
    mOrdered = alloc<double>(size * maxFrames);
    mNumFrames = 0;
}

bool compareLess(double a, double b)
{
    return (a < b || (isnan(b) && !isnan(a)));
}

bool compareMore(double a, double b)
{
    return (a > b || (isnan(a) && !isnan(b)));
}

unsigned long find(double input, double *channel, unsigned long numFrames)
{
    unsigned long gap = numFrames >> 1;
    unsigned long i = gap;
    
    if (!numFrames)
        return 0;
    
    gap = !gap ? 1 : gap;
    
    while (gap && i < numFrames)
    {
        gap >>= 1;
        gap = !gap ? 1 : gap;

        if (compareLess(input, channel[i]))
        {
            if (gap == 1 && (!i || compareMore(input, channel[i - 1])))
                gap = 0;
            else
                i -= gap;
        }
        else if (compareMore(input, channel[i]))
            i += gap;
        else
            break;
    }
    
    return i;
}

// Process

double *FrameLib_TimeMedian::getChannel(unsigned long idx) const
{
    return mOrdered + (idx * getMaxFrames());
}

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
        
        assert(j < mNumFrames && "Value out of place");
        assert(k < mNumFrames && "Value out of place");
    }
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
        
        assert((!mNumFrames && j == 0) || (j < mNumFrames + 1) && "Value out of place");
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
        
        // Copy
       
        std::copy(channel + j + 1, channel + mNumFrames, channel + j);
        
        assert(j < mNumFrames && "Value out of place");
    }
    
    mNumFrames--;
}

void FrameLib_TimeMedian::result(double *output, unsigned long size, Padded pad, unsigned long padSize)
{
    unsigned long numFrames = padSize ? getNumFrames() : mNumFrames;
    
    double percentile = mParameters.getValue(kPercentile);
    double position = (percentile * (numFrames - 1) / 100.0);
    unsigned long percentilePos = std::min(roundToUInt(position), numFrames - 1);
    
    if (padSize)
    {
        for (unsigned long i = 0; i < size; i++)
        {
            const double padValue = pad[i];
            unsigned long pos = find(padValue, getChannel(i), mNumFrames);
            
            if (percentilePos < pos)
                output[i] = getChannel(i)[percentilePos];
            else if (percentilePos >= pos + padSize)
                output[i] = getChannel(i)[percentilePos - padSize];
            else
                output[i] = padValue;
        }
    }
    else
    {
        for (unsigned long i = 0; i < size; i++)
            output[i] = getChannel(i)[percentilePos];
    }
}
