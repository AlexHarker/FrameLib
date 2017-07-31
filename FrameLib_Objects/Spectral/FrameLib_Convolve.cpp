
#include "FrameLib_Convolve.h"
#include "FrameLib_Spectral_Functions.h"

FrameLib_Convolve::FrameLib_Convolve(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    
    mParameters.setInfo(&sParamInfo);

    mParameters.set(serialisedParameters);
    
    unsigned long maxFFTSizeLog2 = ilog2(mParameters.getInt(kMaxLength));
    
    mFFTSetup = hisstools_create_setup_d(maxFFTSizeLog2);
    mMaxFFTSize = 1 << maxFFTSizeLog2;
}

FrameLib_Convolve::~FrameLib_Convolve()
{
    hisstools_destroy_setup_d(mFFTSetup);
}

// Info

const char *FrameLib_Convolve::objectInfo(bool verbose)
{
    return getInfo("Convolve two real frames together, (using frequency domain processing internally): "
                   "The result will be a frame of M + N - 1 where M and N are the sizes of the two inputs respectively",
                   "Convolve two time domain frames together, (using frequency domain processing internally).", verbose);
}

const char *FrameLib_Convolve::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Input #", "Input #", idx, verbose);
}

const char *FrameLib_Convolve::outputInfo(unsigned long idx, bool verbose)
{
    return "Convolved Output";
}

// Parameter Info

FrameLib_Convolve::ParameterInfo FrameLib_Convolve::sParamInfo;

FrameLib_Convolve::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum output length. The output length will be M + N - 1 where M and N are the sizes of the two inputs respectively");
}

// Process

void FrameLib_Convolve::process()
{
    FFT_SPLIT_COMPLEX_D spectrum1;
    FFT_SPLIT_COMPLEX_D spectrum2;
    
    unsigned long FFTSizelog2 = 0;
    unsigned long FFTSize = 0;
    
    // Get Inputs
    
    unsigned long sizeIn1, sizeIn2, sizeOut;
    double *input1 = getInput(0, &sizeIn1);
    double *input2 = getInput(1, &sizeIn2);
    
    // Get FFT size log 2
    
    if (sizeIn1 && sizeIn2)
    {
        sizeOut = sizeIn1 + sizeIn2 - 1;
        FFTSizelog2 = log2(sizeOut);
        FFTSize = 1 << FFTSizelog2;
    }
    else sizeOut = 0;
    
    // Check size
    
    if (FFTSize > mMaxFFTSize)
        sizeOut = 0;
    
    // Get outpu and temporary memory
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
    
    spectrum1.realp = (double *) (sizeOut ? mAllocator->alloc(FFTSize * 2 * sizeof(double)) : NULL);
    spectrum1.imagp = spectrum1.realp + (FFTSize >> 1);
    spectrum2.realp = spectrum1.imagp + (FFTSize >> 1);
    spectrum2.imagp = spectrum2.realp + (FFTSize >> 1);
    
    // Transform
    
    if (spectrum1.realp)
    {
        double scale = 0.5 / (double) FFTSize;
        
        // Take the real ffts
        
        hisstools_unzip_zero_d(input1, &spectrum1, sizeIn1, FFTSizelog2);
        hisstools_rfft_d(mFFTSetup, &spectrum1, FFTSizelog2);
        hisstools_unzip_zero_d(input2, &spectrum2, sizeIn2, FFTSizelog2);
        hisstools_rfft_d(mFFTSetup, &spectrum2, FFTSizelog2);
        
        // Convolve
        
        // Do DC and Nyquist bins
        
        spectrum1.realp[0] *= scale * spectrum2.realp[0];
        spectrum1.imagp[0] *= scale * spectrum2.imagp[0];
        
        // Other bins
        
        for (unsigned long i = 1; i < (FFTSize >> 1); i++)
        {
            double a = spectrum1.realp[i];
            double b = spectrum1.imagp[i];
            double c = spectrum2.realp[i];
            double d = spectrum2.imagp[i];
            
            spectrum1.realp[i] = scale * (a * c - b * d);
            spectrum1.imagp[i] = scale * (a * d + b * c);
        }
        
        // Inverse iFFT
        
        hisstools_rifft_d(mFFTSetup, &spectrum1, FFTSizelog2);
        hisstools_zip_d(&spectrum1, spectrum2.realp, FFTSizelog2);
        
        // Copy
        
        for (unsigned long i = 0; i < sizeOut; i++)
            output[i] = spectrum2.realp[i];
        
        mAllocator->dealloc(spectrum1.realp);
    }
}