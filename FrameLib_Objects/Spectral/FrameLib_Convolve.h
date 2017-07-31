
#ifndef FRAMELIB_CONVOLVE_H
#define FRAMELIB_CONVOLVE_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"

class FrameLib_Convolve : public FrameLib_Processor, private FrameLib_Info
{
    // Parameter Enums and Info

	enum ParameterList { kMaxLength };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor / Destructor
    
    FrameLib_Convolve(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    ~FrameLib_Convolve();
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);

private:

    // Process
    
    void process();
	
private:
    
    // Data
	
	// FFT Setup
	
	FFT_SETUP_D mFFTSetup;
	
	// Maximum FFT Size
	
	unsigned long mMaxFFTSize;
    
    static ParameterInfo sParamInfo;
};

#endif
