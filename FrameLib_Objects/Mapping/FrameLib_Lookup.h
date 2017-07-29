
#ifndef FRAMELIB_LOOKUP_H
#define FRAMELIB_LOOKUP_H

#include "FrameLib_DSP.h"

class FrameLib_Lookup : public FrameLib_Processor
{
    
public:
    
    // Constructor

    FrameLib_Lookup(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process
    
    void process();
};

#endif
