
#ifndef FRAMELIB_REGISTER_H
#define FRAMELIB_REGISTER_H

#include "FrameLib_DSP.h"

class FrameLib_Register : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kMode };
    enum Modes { kStore, kPass };
    
public:
	
    // Constructor
    
    FrameLib_Register(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:

    // Process
    
    void process();
};

#endif