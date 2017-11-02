
#ifndef FRAMELIB_COMBINEPARAM_H
#define FRAMELIB_COMBINEPARAM_H

#include "FrameLib_DSP.h"

class FrameLib_CombineParam : public FrameLib_Processor
{
    const static int maxNumIns = 10;
    
    // Parameter Enums and Info

    enum ParameterList { kNumIns };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_CombineParam(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Process
    
    void process();
    
    // Data
    
    unsigned long mNumIns;
    
    static ParameterInfo sParamInfo;
};

#endif
