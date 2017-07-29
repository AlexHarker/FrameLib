
#include "FrameLib_FFT.h"
#include "FrameLib_Spectral_Functions.h"

// Constructor / Destructor

FrameLib_FFT::FrameLib_FFT(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 2)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    unsigned long maxFFTSizeLog2 = ilog2(mParameters.getInt(kMaxLength));
    
    mFFTSetup = hisstools_create_setup_d(maxFFTSizeLog2);
    mMaxFFTSize = 1 << maxFFTSizeLog2;
}

FrameLib_FFT::~FrameLib_FFT()
{
    hisstools_destroy_setup_d(mFFTSetup);
}

// Process

void FrameLib_FFT::process()
{
    FFT_SPLIT_COMPLEX_D spectrum;
    
    // Get Input
    
    unsigned long sizeIn, sizeOut;
    double *input = getInput(0, &sizeIn);
    
    // Get FFT size log 2
    
    unsigned long FFTSizelog2 = log2(sizeIn);
    unsigned long FFTSize = 1 << FFTSizelog2;
    sizeOut = (FFTSize >> 1) + 1;
    
    // Check size
    
    if (FFTSize > mMaxFFTSize || !sizeIn)
        sizeOut = 0;
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    requestOutputSize(1, sizeOut);
    allocateOutputs();
    spectrum.realp = getOutput(0, &sizeOut);
    spectrum.imagp = getOutput(1, &sizeOut);
    
    // Transform
    
    if (sizeOut && spectrum.realp && spectrum.imagp)
    {
        // Take the real fft
        
        hisstools_unzip_zero_d(input, &spectrum, sizeIn, FFTSizelog2);
        hisstools_rfft_d(mFFTSetup, &spectrum, FFTSizelog2);
        
        // Move Nyquist Bin
        
        spectrum.realp[sizeOut - 1] = spectrum.imagp[0];
        spectrum.imagp[sizeOut - 1] = 0.0;
        spectrum.imagp[0] = 0.0;
        
        // Scale
        
        for (unsigned long i = 0; i < sizeOut; i++)
        {
            spectrum.realp[i] *= 0.5;
            spectrum.imagp[i] *= 0.5;
        }
    }
}
