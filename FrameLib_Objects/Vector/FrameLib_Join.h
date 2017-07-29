
#ifndef FrameLib_Join_H
#define FrameLib_Join_H

#include "FrameLib_DSP.h"

// FIX - time variable triggers

class FrameLib_Join : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kNumIns, kTriggers };
    
public:
	
    // Constructor

    FrameLib_Join(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process
    
    void process();
};

#endif
