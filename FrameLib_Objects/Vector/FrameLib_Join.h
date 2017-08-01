
#ifndef FrameLib_Join_H
#define FrameLib_Join_H

#include "FrameLib_DSP.h"

// FIX - time variable triggers

class FrameLib_Join : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kNumIns, kTriggers };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor

    FrameLib_Join(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process
    
    void process();
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
