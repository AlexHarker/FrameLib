
#ifndef FRAMELIB_TIMEMEDIAN_H
#define FRAMELIB_TIMEMEDIAN_H

#include "FrameLib_TimeBuffer_Template.h"

class FrameLib_TimeMedian final : public FrameLib_TimeBuffer<FrameLib_TimeMedian, 1>
{
    friend FrameLib_TimeBuffer<FrameLib_TimeMedian, 1>;
    
    enum AddedParameterList { kPercentile = kNumFrames + 1 };

    static const char *paramInfo(unsigned long idx);
    
public:
    
    // Constructor

    FrameLib_TimeMedian(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);

    // Info
    
    const char *getOpString() override { return "specified percentile"; }
    const char *getExtraString() override { return "The default percentile is 50% which returns the median. "; }

    // Smooth

    void resetSize(unsigned long maxFrames, unsigned long size) override;

private:

    double *getChannel(unsigned long idx);

    void add(const double *newFrame, unsigned long size) override;
    void remove(const double *oldFrame, unsigned long size) override;
    void exchange(const double *newFrame, const double *oldFrame, unsigned long size) override;
    void result(double *output, unsigned long size, Padded pad, unsigned long padSize) override;
    
    // Data
        
    AutoArray<double> mOrdered;
    unsigned long mNumFrames;
};

#endif
