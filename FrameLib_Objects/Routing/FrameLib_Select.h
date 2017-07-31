
#ifndef FRAMELIB_SELECT_H
#define FRAMELIB_SELECT_H

#include "FrameLib_DSP.h"

class FrameLib_Select : public FrameLib_Processor, private FrameLib_Info
{
    // Parameter Enums and Info

    enum ParameterList {kNumIns, kActiveIn};

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Select(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);
    

private:
    
    // Update and Process
    
    void update();
    void process();
    
    // Data
    
    long mNumIns;
    long mActiveIn;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
