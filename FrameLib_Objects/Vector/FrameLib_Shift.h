
#ifndef FRAMELIB_SHIFT_H
#define FRAMELIB_SHIFT_H

#include "FrameLib_DSP.h"

class FrameLib_Shift : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kShift, kPadding, kMode, kUnits };
    enum Modes { kPad, kWrap };
    enum Units { kSamples, kRatio };
    
public:
	
    // Constructor

    FrameLib_Shift(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process
    
    void process();
};

#endif
