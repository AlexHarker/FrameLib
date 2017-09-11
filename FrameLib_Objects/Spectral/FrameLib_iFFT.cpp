
#include "FrameLib_iFFT.h"
#include "FrameLib_Spectral_Functions.h"

// Constructor / Destructor

FrameLib_iFFT::FrameLib_iFFT(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, &sParamInfo, 2, 1)
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

FrameLib_iFFT::~FrameLib_iFFT()
{
    hisstools_destroy_setup(mFFTSetup);
}

// Info

std::string FrameLib_iFFT::objectInfo(bool verbose)
{
    return getInfo("Calculate the inverse real Fast Fourier Transform of two input frames (comprising the real and imaginary values): All FFTs performed will use a power of two size. "
                   "Output frames will be N in length where N is the FFT size. Inputs are expected to match in length with a length of (N / 2) + 1.",
                   "Calculate the inverse real Fast Fourier Transform of two input frames (comprising the real and imaginary values).", verbose);
}

std::string FrameLib_iFFT::inputInfo(unsigned long idx, bool verbose)
{
    if (!idx)
        return getInfo("Frequency Domain Real Values - inputs should match in size and be (N / 2) + 1 in length.", "Freq Domain Real Values", verbose);
    else
        return getInfo("Frequency Domain Imaginary Values - inputs should match in size and be (N / 2) + 1 in length.", "Freq Domain Imag Values", verbose);
}

std::string FrameLib_iFFT::outputInfo(unsigned long idx, bool verbose)
{
    return "Time Domain Output";
}

// Parameter Info

FrameLib_iFFT::ParameterInfo FrameLib_iFFT::sParamInfo;

FrameLib_iFFT::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum output length / FFT size.");
    add("When on the input is expected to be normalised.");
}

// Process

void FrameLib_iFFT::process()
{
    FFT_SPLIT_COMPLEX_D spectrum;
    
    unsigned long sizeIn1, sizeIn2, sizeIn, sizeOut;
    unsigned long FFTSizelog2 = 0;
    
    // Get Inputs
    
    double *input1 = getInput(0, &sizeIn1);
    double *input2 = getInput(1, &sizeIn2);
    
    sizeIn = sizeIn1 < sizeIn2 ? sizeIn1 : sizeIn2;
    
    if (sizeIn)
    {
        FFTSizelog2 = ilog2((sizeIn - 1) << 1);
        sizeOut = 1 << FFTSizelog2;
    }
    else
        sizeOut = 0;
    
    // Sanity Check
    
    if (sizeOut > mMaxFFTSize)
        sizeOut = 0;
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
    
    spectrum.realp = (double *) (sizeOut ? mAllocator->alloc(sizeOut * sizeof(double)) : NULL);
    spectrum.imagp = spectrum.realp + (sizeOut >> 1);
    
    if (spectrum.realp)
    {
        double scale = mParameters.getBool(kNormalise) ? 1.0 : 1.0 / (double) sizeOut;
        
        // Copy Spectrum
        
        for (unsigned long i = 0; (i < sizeIn) && (i < (sizeOut >> 1)); i++)
        {
            spectrum.realp[i] = input1[i];
            spectrum.imagp[i] = input2[i];
        }
        
        if (sizeIn == (sizeOut >> 1) + 1)
            spectrum.imagp[0] = input1[sizeOut >> 1];
        
        // Convert to time domain
        
        hisstools_rifft(mFFTSetup, &spectrum, output, FFTSizelog2);
        
        // Scale
        
        for (unsigned long i = 0; i < sizeOut; i++)
            output[i] *= scale;
        
        mAllocator->dealloc(spectrum.realp);
    }
}
