
#ifndef FRAMELIB_EWMA_H
#define FRAMELIB_EWMA_H

#include "FrameLib_DSP.h"

class FrameLib_EWMA : public FrameLib_Processor
{
    // Parameter Enums and Info
    
    enum ParameterList { kAlphaUp, kAlphaDown };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_EWMA(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    ~FrameLib_EWMA()
    {
        dealloc(mAverageFrame);
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
    double *mPrevFrame;
    unsigned long mFrameSize;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
