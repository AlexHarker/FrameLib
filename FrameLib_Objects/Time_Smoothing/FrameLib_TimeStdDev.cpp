
#include "FrameLib_TimeStdDev.h"

// Constructor / Destructor

FrameLib_TimeStdDev::FrameLib_TimeStdDev(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_TimeBuffer<FrameLib_TimeStdDev>(context, serialisedParameters, proxy)
, mSum(nullptr)
, mSqSum(nullptr)
{}

FrameLib_TimeStdDev::~FrameLib_TimeStdDev()
{
    dealloc(mSum);
    dealloc(mSqSum);
}

// Info

std::string FrameLib_TimeStdDev::objectInfo(bool verbose)
{
    return formatInfo("Outputs the standard deviation per sample over a given number of frames: Frames are expected to be of uniform size, otherwise the buffer is reset. The number of frames (as well as the maximum number of frames) can be set as parameters. The output is the same size as the input.",
                      "Outputs the standard deviation per sample over a given number of frames.", verbose);
}

std::string FrameLib_TimeStdDev::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:     return "Input";
        case 1:     return "Reset Input";
        default:    return parameterInputInfo(verbose);
    }
}

std::string FrameLib_TimeStdDev::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return "Buffer Full";
    else
        return "Output";
}

// Update size

void FrameLib_TimeStdDev::resetSize(unsigned long maxFrames, unsigned long size)
{
    dealloc(mSum);
    dealloc(mSqSum);
    
    mSum = alloc<NeumaierSum>(size);
    mSqSum = alloc<NeumaierSum>(size);

    for (unsigned long i = 0; i < size; i++)
        mSum[i].clear();
    
    for (unsigned long i = 0; i < size; i++)
        mSqSum[i].clear();
}

// Process

void FrameLib_TimeStdDev::add(const double *newFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
    {
        mSum[i].sum(newFrame[i]);
        mSqSum[i].sum(newFrame[i] * newFrame[i]);
    }
}

void FrameLib_TimeStdDev::remove(const double *oldFrame, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
    {
        mSum[i].sum(-oldFrame[i]);
        mSqSum[i].sum(-(oldFrame[i] * oldFrame[i]));
    }
}

void FrameLib_TimeStdDev::result(double *output, unsigned long size, const PaddedVector& pad, unsigned long padSize)
{
    double r1 = 1.0 / getNumFrames(true);
    double r2 = 1.0 / getNumFrames(false);

    for (unsigned long i = 0; i < size; i++)
    {
        // N.B. population variance = (sqSum - (sum * sum / N)) / N
        
        const double sum = mSum[i].value();
        const double sqSum = mSqSum[i].value();
        const double padValue = pad[i];
        const double padSum = padValue * padValue * padSize;
        
        const double variance = std::max((padSum + sqSum - ((sum * sum) * r1)) * r2, 0.0);
        
        output[i] = sqrt(variance);        
    }
}
