
#include "FrameLib_Multitaper.h"

// Constructor / Destructor

FrameLib_Multitaper::FrameLib_Multitaper(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 1, 1), Spectral_Processor(context)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    
    mParameters.addInt(kNumTapers, "numtapers", 3, 1);
    mParameters.setMin(1);
    
    mParameters.set(serialisedParameters);
    
    setMaxFFTSize(mParameters.getInt(kMaxLength) * 2);
}

// Helpers

void FrameLib_Multitaper::getWrapped(double &rOut, double&iOut, double *real, double *imag, long size, long pos)
{
    pos &= (size - 1);
    
    if (pos <= (size >> 1))
    {
        rOut = real[pos];
        iOut = imag[pos];
    }
    else
    {
        pos = size - pos;
        rOut = real[pos];
        iOut = -imag[pos];
    }
}

// Info

std::string FrameLib_Multitaper::objectInfo(bool verbose)
{
    return formatInfo("Calculates the multitaper power spectrum of a real input using the cosine tapers: All FFTs performed will use a power of two size. "
                   "Output frames will be (N / 2) + 1 in length where N is the FFT size. Inputs which are not a power of two are zero-padded to the next power of two.",
                   "Calculates the multitaper power spectrum of a real input using the cosinre tapers.", verbose);
}

std::string FrameLib_Multitaper::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return formatInfo("Frequency Domain Real Values - inputs should match in size and be (N / 2) + 1 in length.", "Freq Domain Real Values", verbose);
    else
        return formatInfo("Frequency Domain Imaginary Values - inputs should match in size and be (N / 2) + 1 in length.", "Freq Domain Imag Values", verbose);
}

std::string FrameLib_Multitaper::outputInfo(unsigned long idx, bool verbose)
{
    return "Time Domain Output";
}

// Parameter Info

FrameLib_Multitaper::ParameterInfo FrameLib_Multitaper::sParamInfo;

FrameLib_Multitaper::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum input length / FFT size.");
    add("Sets the number of tapers to use.");
}

// Process

void FrameLib_Multitaper::process()
{
    // Get Input
    
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    
    // Get FFT size log 2
    
    unsigned long FFTSizelog2 = ilog2(sizeIn);
    unsigned long FFTSize = 1 << FFTSizelog2;
    sizeOut = (FFTSize >> 1) + 1;
    
    // Check size
    
    if ((FFTSize * 2) > maxFFTSize() || !sizeIn)
        sizeOut = 0;
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    double *tempMem = alloc<double>((FFTSize + 1) << 1);
    
    unsigned long nTapers = mParameters.getInt(kNumTapers);
    
    // Transform
    
    if (tempMem && sizeOut && output)
    {
        FFT_SPLIT_COMPLEX_D spectrum;
        
        spectrum.realp = ((double *) tempMem);
        spectrum.imagp = ((double *) tempMem) + (FFTSize + 1);
        
        // Take the real fft
        
        transformForwardReal(spectrum, input, sizeIn, (FFTSizelog2 + 1));
        
        // Move Nyquist Bin
        
        spectrum.realp[FFTSize] = spectrum.imagp[0];
        spectrum.imagp[FFTSize] = 0.0;
        spectrum.imagp[0] = 0.0;
        
        // Scale
        
        scaleSpectrum(spectrum, FFTSize + 1, 0.5);
        
        // Do Multitaper
        
        // Zero output power spectrum
        
        zeroVector(output, sizeOut);

        // Do tapers
        
        // FIX - weighting - I can't find where this stuff comes from...
        
        double weightSum = nTapers - (((1.0 / (double) nTapers) - 3.0 + 2.0 * nTapers) / 6.0);
        double normFactor = 1.0 / (4.0 * weightSum);
        
        for (unsigned long i = 1; i <= nTapers; i++)
        {
            double weight = (1.0 - ((i - 1) * (i - 1)) / (double) (nTapers * nTapers));
            double taperScale = weight * normFactor;
            
            // Deal with bins safely (could be optimised)
            
            for (unsigned long j = 0; j < sizeOut; j++)
            {
                double r1, i1, r2, i2, r3, i3;
                
                getWrapped(r1, i1, spectrum.realp, spectrum.imagp, FFTSize << 1, ((j << 1) + i));
                getWrapped(r2, i2, spectrum.realp, spectrum.imagp, FFTSize << 1, ((j << 1) - i));
                
                // FIX - possible these should be flipped...
                
                r3 = (i1 - i2);
                i3 = (r2 - r1);
                
                output[j] += ((r3 * r3) + (i3 * i3)) * taperScale;
            }
        }
        
    }
    
    dealloc(tempMem);
}
