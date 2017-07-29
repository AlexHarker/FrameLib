
#ifndef FRAMELIB_SPLIT_H
#define FRAMELIB_SPLIT_H

#include "FrameLib_DSP.h"

// FIX - consider multi-output version later

class FrameLib_Split : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kSplit, kUnits };
    enum Units { kSamples, kRatio };
    
public:
	
    // Constructor
    
    FrameLib_Split(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process

    void process();
};

#endif
