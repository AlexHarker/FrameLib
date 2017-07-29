
#ifndef FRAMELIB_SETPARAM_H
#define FRAMELIB_SETPARAM_H

#include "FrameLib_DSP.h"

class FrameLib_SetParam : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kNumIns, kNames };
    
public:
    
    // Constructor
    
    FrameLib_SetParam (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process
    
    void process();
    
    // Data
    
    unsigned long mNumIns;
};

#endif
