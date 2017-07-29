
#ifndef FRAMELIB_COUNT_H
#define FRAMELIB_COUNT_H

#include "FrameLib_DSP.h"

class FrameLib_Count : public FrameLib_Processor, private FrameLib_Info
{
    // Parameter Enums and Info
    
    enum ParameterList { kLength, kMode };
    enum Modes { kInLength, kRequestedLength };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Count(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);

private:
    
    // Update and Process
    
    void update();
    void process();
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
