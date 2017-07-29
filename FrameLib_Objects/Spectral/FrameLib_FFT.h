
#ifndef FRAMELIB_FFT_H
#define FRAMELIB_FFT_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"

// FIX - add zero padding (why not do this prior to FFT with the pad object?)

class FrameLib_FFT : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kMaxLength };
    
public:
	
    // Constructor / Destructor

    FrameLib_FFT(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    ~FrameLib_FFT();
	
private:

    // Process
    
    void process();
	
    // Data
    
	// FFT Setup
	
	FFT_SETUP_D mFFTSetup;
	
	// Maximum FFT Size
	
	unsigned long mMaxFFTSize;
};

#endif
