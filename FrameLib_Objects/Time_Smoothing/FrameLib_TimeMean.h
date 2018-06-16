
#ifndef FrameLib_TimeMean_H
#define FrameLib_TimeMean_H

#include "FrameLib_TimeBuffer_Template.h"

class FrameLib_TimeMean final : public FrameLib_TimeBuffer<FrameLib_TimeMean>
{
    
public:
    
    // Constructor
    
    FrameLib_TimeMean(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);

    ~FrameLib_TimeMean()
    {
        dealloc(mSum);
        dealloc(mCompensate);
    }

    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
    // Smooth

    void resetSize(unsigned long maxFrames, unsigned long size) override;

    void add(const double *newFrame, unsigned long size) override;
    void remove(const double *oldFrame, unsigned long size) override;
    void result(double *output, unsigned long size) override;
    
private:
    
    // Object Reset
    
    void objectReset() override { smoothReset(); }
    
    // Data
    
    double *mSum;
    double *mCompensate;    
};

#endif
