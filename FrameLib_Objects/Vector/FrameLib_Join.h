
#ifndef FrameLib_Join_H
#define FrameLib_Join_H

#include "FrameLib_DSP.h"

// FIX - time variable triggers

class FrameLib_Join : public FrameLib_Processor, private FrameLib_Info
{
    // Parameter Enums and Info

	enum ParameterList { kNumIns, kTriggers };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor

    FrameLib_Join(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process
    
    void process();
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
