
#include "FrameLib_TimeMean.h"

// Constructor / Destructor

FrameLib_TimeMean::FrameLib_TimeMean(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_TimeBuffer<FrameLib_TimeMean>(context, serialisedParameters, proxy)
, mSum(nullptr)
{}

FrameLib_TimeMean::~FrameLib_TimeMean()
{
    dealloc(mSum);
}

// Info

std::string FrameLib_TimeMean::objectInfo(bool verbose)
{
    return formatInfo("Outputs the mean per sample over a given number of frames: Frames are expected to be of uniform size, otherwise the buffer is reset. The number of frames (as well as the maximum number of frames) can be set as parameters. The output is the same size as the input.", "Outputs the mean per sample over a given number of frames.", verbose);
}

std::string FrameLib_TimeMean::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:     return "Input";
        case 1:     return "Reset Input";
        default:    return parameterInputInfo(verbose);
    }
}

std::string FrameLib_TimeMean::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return "Buffer Full";
    else
        return "Output";
}

// Update size

void FrameLib_TimeMean::resetSize(unsigned long maxFrames, unsigned long size)
{
    dealloc(mSum);
    mSum = alloc<NeumaierSum>(size);
    
    for (unsigned long i = 0; i < size; i++)
        mSum[i].clear();
}

// Process

void FrameLib_TimeMean::add(const double *newFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        mSum[i].sum(newFrame[i]);
}

void FrameLib_TimeMean::remove(const double *oldFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        mSum[i].sum(-oldFrame[i]);
}

void FrameLib_TimeMean::result(double *output, unsigned long size, Padded pad, unsigned long padSize)
{
    const double recip = 1.0 / getNumFrames();
    
    for (unsigned long i = 0; i < size; i++)
        output[i] = (pad[i] * padSize + mSum[i].value()) * recip;
}
