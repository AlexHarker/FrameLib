
#ifndef FRAMELIB_UNTAG_H
#define FRAMELIB_UNTAG_H

#include "FrameLib_DSP.h"

class FrameLib_Untag : public FrameLib_Processor
{
    const static int maxNumOuts = 10;
    
    // Parameter Enums and Info

    enum ParameterList { kNumOuts, kNames };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Untag(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Process
    
    void process();
    
    // Data
    
    unsigned long mNumOuts;
    
    static ParameterInfo sParamInfo;
};

#endif
