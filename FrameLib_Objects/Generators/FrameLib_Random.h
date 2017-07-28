
#ifndef FRAMELIB_RANDOM_H
#define FRAMELIB_RANDOM_H

#include "FrameLib_DSP.h"
#include "FrameLib_RandGen.h"

class FrameLib_Random : public FrameLib_Processor, private FrameLib_Info
{
    // Parameter enums and info
    
    enum ParameterList { kLength, kMode };
    enum Modes { kInLength, kRequestedLength };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor
    
    FrameLib_Random(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Process
    
    void process();

    // Data
    
    FrameLib_RandGen mRandom;
    static ParameterInfo sParamInfo;
};

#endif
