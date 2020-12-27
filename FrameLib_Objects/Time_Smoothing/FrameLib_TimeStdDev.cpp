
#include "FrameLib_TimeStdDev.h"

// Constructor

FrameLib_TimeStdDev::FrameLib_TimeStdDev(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_TimeBuffer<FrameLib_TimeStdDev>(context, serialisedParameters, proxy)
{}

// Update size

void FrameLib_TimeStdDev::resetSize(unsigned long maxFrames, unsigned long size)
{
    mSum = allocAutoArray<NeumaierSum>(size);
    mSqSum = allocAutoArray<NeumaierSum>(size);

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

void FrameLib_TimeStdDev::result(double *output, unsigned long size, Padded pad, unsigned long padSize)
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
