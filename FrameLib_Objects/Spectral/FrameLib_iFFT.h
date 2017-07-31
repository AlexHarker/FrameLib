
#ifndef FRAMELIB_IFFT_H
#define FRAMELIB_IFFT_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"

// FIX - review FFTSetup

class FrameLib_iFFT : public FrameLib_Processor, private FrameLib_Info
{
    // Parameter Enums and Info

	enum ParameterList { kMaxLength };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor / Destructor

    FrameLib_iFFT(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);    
    ~FrameLib_iFFT();
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);

private:

	// Process
    
    void process();
	
    // Data
    
	// FFT Setup
	
	FFT_SETUP_D mFFTSetup;
	
	// Maximum FFT Size
	
	unsigned long mMaxFFTSize;
    
    static ParameterInfo sParamInfo;
};

#endif
