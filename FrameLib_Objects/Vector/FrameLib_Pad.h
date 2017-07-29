
#ifndef FRAMELIB_PAD_H
#define FRAMELIB_PAD_H

#include "FrameLib_DSP.h"

class FrameLib_Pad : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kPadding, kStart, kEnd, kUnits };
    enum Units { kSamples, kRatio };
    
public:
	
    // Constructor
    
    FrameLib_Pad(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process
    
    void process ();
};

#endif
