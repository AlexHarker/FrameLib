
#ifndef FRAMELIB_TIMEMEDIAN_H
#define FRAMELIB_TIMEMEDIAN_H

#include "FrameLib_TimeBuffer_Template.h"

class FrameLib_TimeMedian final : public FrameLib_TimeBuffer<FrameLib_TimeMedian, true>
{
   
public:
    
    // Constructor
    
    FrameLib_TimeMedian(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);

    ~FrameLib_TimeMedian()
    {
        dealloc(mOrdered);
    }

    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
    // Smooth

    void resetSize(unsigned long maxFrames, unsigned long size) override;

private:

    double *getChannel(unsigned long idx) const { return mOrdered + (idx * getMaxFrames()); }

    void add(const double *newFrame, unsigned long size) override;
    void remove(const double *oldFrame, unsigned long size) override;
    void exchange(const double *newFrame, const double *oldFrame, unsigned long size) override;
    void result(double *output, unsigned long size) override;
    
    // Object Reset
    
    void objectReset() override { smoothReset(); }
    
    // Data
        
    double *mOrdered;
    unsigned long mNumFrames;
};

#endif
