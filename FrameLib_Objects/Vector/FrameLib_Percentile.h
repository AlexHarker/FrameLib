
#ifndef FRAMELIB_PRECENTILE_H
#define FRAMELIB_PRECENTILE_H

#include "FrameLib_DSP.h"

class FrameLib_Percentile : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kPercentile };
    
public:
    
    // Constructor
    
    FrameLib_Percentile(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:

    // Process

    void process();
    
    // Data

    double mPercentile;
};

#endif