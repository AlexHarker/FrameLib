
#ifndef FRAMELIB_SORT_H
#define FRAMELIB_SORT_H

#include "FrameLib_DSP.h"

class FrameLib_Sort : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kOrder, kOutputIndices };
    enum Orders { kUp, kDown };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Sort(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
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
