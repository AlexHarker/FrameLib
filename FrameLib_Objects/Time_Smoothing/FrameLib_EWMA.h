
#ifndef FRAMELIB_EWMA_H
#define FRAMELIB_EWMA_H

#include "FrameLib_DSP.h"

class FrameLib_EWMA final : public FrameLib_Processor
{
    // Parameter Enums and Info
    
    enum ParameterList { kAlphaUp, kAlphaDown };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_EWMA(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    ~FrameLib_EWMA()
    {
        dealloc(mAverageFrame);
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
    double *mPrevFrame;
    unsigned long mFrameSize;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
