#ifndef FRAMELIB_REV_H
#define FRAMELIB_REV_H

#include "FrameLib_DSP.h"

class FrameLib_Rev : public FrameLib_Processor
{
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor

    FrameLib_Rev(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
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