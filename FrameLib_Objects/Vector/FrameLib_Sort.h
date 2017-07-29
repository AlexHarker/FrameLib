
#ifndef FRAMELIB_SORT_H
#define FRAMELIB_SORT_H

#include "FrameLib_DSP.h"

class FrameLib_Sort : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kOrder };
    enum Orders { kUp, kDown };

public:
    
    // Constructor
    
    FrameLib_Sort(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process

    void process();
    
    // Data

    Orders mOrder;
};

#endif
