
#ifndef FRAMELIB_PRECENTILE_H
#define FRAMELIB_PRECENTILE_H

#include "FrameLib_DSP.h"

class FrameLib_Percentile : public FrameLib_Processor, private FrameLib_Info
{
    // Parameter Enums and Info

    enum ParameterList { kPercentile };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Percentile(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);

private:

    // Process

    void process();
    
    // Data

    double mPercentile;
    
    static ParameterInfo sParamInfo;
};

#endif