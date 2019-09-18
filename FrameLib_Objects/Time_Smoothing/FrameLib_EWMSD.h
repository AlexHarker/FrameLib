
#ifndef FRAMELIB_EWMSD_H
#define FRAMELIB_EWMSD_H

#include "FrameLib_DSP.h"

class FrameLib_EWMSD final : public FrameLib_Processor
{
    // Parameter Enums and Info
    
    enum ParameterList { kAlphaUp, kAlphaDown };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_EWMSD(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    ~FrameLib_EWMSD()
    {
        dealloc(mAverageFrame);
        dealloc(mVarianceFrame);
        dealloc(mPrevFrame);
    }
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:

    // Object Reset
    
    void objectReset() override { mFrameSize = 0; }
    
    // Process
    
    void process() override;
    
    // Data

    double *mAverageFrame;
    double *mVarianceFrame;
    double *mPrevFrame;
    unsigned long mFrameSize;
    
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
