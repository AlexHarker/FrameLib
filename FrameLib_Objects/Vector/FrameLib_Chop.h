
#ifndef FRAMELIB_CHOP_H
#define FRAMELIB_CHOP_H

#include "FrameLib_DSP.h"

class FrameLib_Chop : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kNumOuts, kSize, kUnits };
    enum Units { kSamples, kRatio };
    
public:
	
    // Constructor
    
    FrameLib_Chop(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process
    
    void process();
    
    // Data
    
    unsigned long mNumOuts;
};

#endif
