
#include "FrameLib_FFT.h"
#include "FrameLib_Spectral_Functions.h"

// Constructor / Destructor

FrameLib_FFT::FrameLib_FFT(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, &sParamInfo, 1, 2)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    mParameters.addBool(kNormalise, "normalise", false, 1);
    mParameters.setInstantiation();

    mParameters.set(serialisedParameters);
    
    unsigned long maxFFTSizeLog2 = ilog2(mParameters.getInt(kMaxLength));
    
    hisstools_create_setup(&mFFTSetup, maxFFTSizeLog2);
    mMaxFFTSize = 1 << maxFFTSizeLog2;
}

FrameLib_FFT::~FrameLib_FFT()
{
    hisstools_destroy_setup(mFFTSetup);
}

// Info

std::string FrameLib_FFT::objectInfo(bool verbose)
{
    return getInfo("Calculate the real Fast Fourier Transform of an input frame: All FFTs performed will use a power of two size. "
                   "Output frames will be (N / 2) + 1 in length where N is the FFT size. Inputs which are not a power of two are zero-padded to the next power of two. "
                   "Real and imaginary values are output as separate frames.",
                   "Calculate the real Fast Fourier Transform of an input frame.", verbose);
}

std::string FrameLib_FFT::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Time Domain Input - will be zero-padded if the length is not a power of two.", "Time Domain Input", verbose);
}

std::string FrameLib_FFT::outputInfo(unsigned long idx, bool verbose)
{
    if (!idx)
        return "Real Values";
    else
        return "Imaginary Values";
}

// Parameter Info

FrameLib_FFT::ParameterInfo FrameLib_FFT::sParamInfo;

FrameLib_FFT::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum input length / FFT size.");
    add("When on the output is normalised so that sine waves produce the same level output regardless of the FFT size.");
}

// Process

void FrameLib_FFT::process()
{
    FFT_SPLIT_COMPLEX_D spectrum;
    
    // Get Input
    
    unsigned long sizeIn, sizeOut;
    double *input = getInput(0, &sizeIn);
    
    // Get FFT size log 2
    
    unsigned long FFTSizelog2 = ilog2(sizeIn);
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
        
        hisstools_rfft(mFFTSetup, input, &spectrum, sizeIn, FFTSizelog2);
        
        // Move Nyquist Bin
        
        spectrum.realp[sizeOut - 1] = spectrum.imagp[0];
        spectrum.imagp[sizeOut - 1] = 0.0;
        spectrum.imagp[0] = 0.0;
        
        // Scale
        
        double scale = mParameters.getBool(kNormalise) ? 1.0 / (double) FFTSize : 0.5;
        
        for (unsigned long i = 0; i < sizeOut; i++)
        {
            spectrum.realp[i] *= scale;
            spectrum.imagp[i] *= scale;
        }
    }
}
