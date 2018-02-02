
#ifndef FRAMELIB_ACCUMPOINT_H
#define FRAMELIB_ACCUMPOINT_H

#include "FrameLib_DSP.h"

class FrameLib_AccumPoint : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kPercentage };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_AccumPoint(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
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
