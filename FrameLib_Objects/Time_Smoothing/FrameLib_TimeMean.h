
#ifndef FrameLib_TimeMean_H
#define FrameLib_TimeMean_H

#include "FrameLib_TimeBuffer_Template.h"
#include "FrameLib_NeumaierSum.h"

class FrameLib_TimeMean final : public FrameLib_TimeBuffer<FrameLib_TimeMean>
{
public:
    
    // Constructor
    
    FrameLib_TimeMean(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);

    // Info
    
    const char *getOpString() override { return "mean"; }
    
    // Smooth

    void resetSize(unsigned long maxFrames, unsigned long size) override;

    void add(const double *newFrame, unsigned long size) override;
    void remove(const double *oldFrame, unsigned long size) override;
    void result(double *output, unsigned long size, Padded pad, unsigned long padSize) override;
    
private:
    
    // Data
    
    AutoArray<NeumaierSum> mSum;
};

#endif
