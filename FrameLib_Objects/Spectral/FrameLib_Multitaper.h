
#ifndef FRAMELIB_MULTITAPER_H
#define FRAMELIB_MULTITAPER_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"

class FrameLib_Multitaper : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kMaxLength, kNumTapers };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor / Destructor

    FrameLib_Multitaper(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    ~FrameLib_Multitaper();
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Helpers
    
    void getWrapped(double &rOut, double&iOut, double *real, double *imag, long size, long pos);
    
    // Process
    
    void process();
	
protected:
	
	// FFT Setup
	
	FFT_SETUP_D mFFTSetup;
	
	// Maximum FFT Size
	
	unsigned long mMaxFFTSize;
    
    static ParameterInfo sParamInfo;
};

#endif
