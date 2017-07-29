
#ifndef FRAMELIB_SUBFRAME_H
#define FRAMELIB_SUBFRAME_H

#include "FrameLib_DSP.h"

class FrameLib_Subframe : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kStart, kEnd, kUnits };
    enum Units { kSamples, kRatio };
    
public:
	
    // Constructor

    FrameLib_Subframe(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process
    
    void process();
};

#endif
