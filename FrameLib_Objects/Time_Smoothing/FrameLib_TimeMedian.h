
#ifndef FRAMELIB_TIMEMEDIAN_H
#define FRAMELIB_TIMEMEDIAN_H

#include "FrameLib_TimeBuffer_Template.h"

class FrameLib_TimeMedian final : public FrameLib_TimeBuffer<FrameLib_TimeMedian, 1>
{
    enum AddedParameterList { kPercentile = kNumFrames + 1 };

public:
    
    // Constructor / Destructor

    FrameLib_TimeMedian(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);

    ~FrameLib_TimeMedian();

    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
    // Smooth

    void resetSize(unsigned long maxFrames, unsigned long size) override;

private:

    double *getChannel(unsigned long idx) const;

    void add(const double *newFrame, unsigned long size) override;
    void remove(const double *oldFrame, unsigned long size) override;
    void exchange(const double *newFrame, const double *oldFrame, unsigned long size) override;
    void result(double *output, unsigned long size, double pad, unsigned long padSize) override;
    
    // Data
        
    double *mOrdered;
    unsigned long mNumFrames;
};

#endif
