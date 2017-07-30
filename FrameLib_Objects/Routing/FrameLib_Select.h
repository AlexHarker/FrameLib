
#ifndef FRAMELIB_SELECT_H
#define FRAMELIB_SELECT_H

#include "FrameLib_DSP.h"

class FrameLib_Select : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList {kNumIns, kActiveIn};

public:
    
    // Constructor
    
    FrameLib_Select(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Update and Process
    
    void update();
    void process();
    
    // Data
    
    long mNumIns;
    long mActiveIn;
};

#endif
