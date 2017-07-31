
#ifndef FRAMELIB_SETPARAM_H
#define FRAMELIB_SETPARAM_H

#include "FrameLib_DSP.h"

class FrameLib_SetParam : public FrameLib_Processor, private FrameLib_Info
{
    // Parameter Enums and Info

    enum ParameterList { kNumIns, kNames };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_SetParam (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Process
    
    void process();
    
    // Data
    
    unsigned long mNumIns;
    
    static ParameterInfo sParamInfo;
};

#endif
