
#ifndef FRAMELIB_SORT_H
#define FRAMELIB_SORT_H

#include "FrameLib_DSP.h"

class FrameLib_Sort : public FrameLib_Processor, private FrameLib_Info
{
    // Parameter Enums and Info

    enum ParameterList { kOrder };
    enum Orders { kUp, kDown };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Sort(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process

    void process();
    
    // Data

    Orders mOrder;
    
    static ParameterInfo sParamInfo;
};

#endif
