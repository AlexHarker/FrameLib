
#ifndef FRAMELIB_EWMSD_H
#define FRAMELIB_EWMSD_H

#include "FrameLib_DSP.h"

class FrameLib_EWMSD : public FrameLib_Processor
{
    // Parameter Enums and Info
    
    enum ParameterList { kAlphaUp, kAlphaDown };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_EWMSD(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    ~FrameLib_EWMSD()
    {
        dealloc(mAverageFrame);
        dealloc(mVarianceFrame);
        dealloc(mPrevFrame);
    }
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:

    // Object Reset
    
    void objectReset() { mFrameSize = 0; }
    
    // Process
    
    void process();
    
    // Data

    double *mAverageFrame;
    double *mVarianceFrame;
    double *mPrevFrame;
    unsigned long mFrameSize;
    
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
