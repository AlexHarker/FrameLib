
#ifndef FRAMELIB_IFFT_H
#define FRAMELIB_IFFT_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"

// FIX - review FFTSetup

class FrameLib_iFFT : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum Mode { kReal, kComplex, kFullSpectrum };
	enum ParameterList { kMaxLength, kNormalise, kMode };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor / Destructor

    FrameLib_iFFT(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);    
    ~FrameLib_iFFT();
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:

	// Process
    
    void process();
	
    // Data
    
	// FFT Setup
	
	FFT_SETUP_D mFFTSetup;
	
    // Instantiation Params
    
    unsigned long mMaxFFTSize;
    Mode mMode;
    bool mNormalise;
    
    static ParameterInfo sParamInfo;
};

#endif
