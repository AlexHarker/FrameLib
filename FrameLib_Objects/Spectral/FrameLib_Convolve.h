
#ifndef FRAMELIB_CONVOLVE_H
#define FRAMELIB_CONVOLVE_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"

class FrameLib_Convolve : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kMaxLength };
    
public:
	
    // Constructor / Destructor
    
    FrameLib_Convolve(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    ~FrameLib_Convolve();
    
private:

    // Process
    
    void process();
	
private:
    
    // Data
	
	// FFT Setup
	
	FFT_SETUP_D mFFTSetup;
	
	// Maximum FFT Size
	
	unsigned long mMaxFFTSize;
};

#endif
