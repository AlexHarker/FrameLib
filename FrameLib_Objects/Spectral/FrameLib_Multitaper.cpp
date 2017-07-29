
#include "FrameLib_Multitaper.h"
#include "FrameLib_Spectral_Functions.h"

// Constructor / Destructor

FrameLib_Multitaper::FrameLib_Multitaper(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    
    mParameters.addInt(kNumTapers, "numtapers", 3, 1);
    mParameters.setMin(1);
    
    mParameters.set(serialisedParameters);
    
    unsigned long maxFFTSizeLog2 = ilog2(mParameters.getInt(kMaxLength));
    
    mFFTSetup = hisstools_create_setup_d(maxFFTSizeLog2 + 1);
    mMaxFFTSize = 1 << maxFFTSizeLog2;
}

FrameLib_Multitaper::~FrameLib_Multitaper()
{
    hisstools_destroy_setup_d(mFFTSetup);
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

// Process

void FrameLib_Multitaper::process()
{
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
    allocateOutputs();
    
    double *tempMem = (double *) mAllocator->alloc(sizeof(double) * ((FFTSize + 1) << 1));
    double *output = getOutput(0, &sizeOut);
    
    long nTapers = mParameters.getInt(kNumTapers);
    
    // Transform
    
    if (tempMem && sizeOut && output)
    {
        FFT_SPLIT_COMPLEX_D spectrum;
        
        spectrum.realp = ((double *) tempMem);
        spectrum.imagp = ((double *) tempMem) + (FFTSize + 1);
        
        // Take the real fft
        
        hisstools_unzip_zero_d(input, &spectrum, sizeIn, (FFTSizelog2 + 1));
        hisstools_rfft_d(mFFTSetup, &spectrum, (FFTSizelog2 + 1));
        
        // Move Nyquist Bin
        
        spectrum.realp[FFTSize] = spectrum.imagp[0];
        spectrum.imagp[FFTSize] = 0.0;
        spectrum.imagp[0] = 0.0;
        
        // Scale
        
        for (unsigned long i = 0; i < (FFTSize + 1); i++)
        {
            spectrum.realp[i] *= 0.5;
            spectrum.imagp[i] *= 0.5;
        }
        
        // Do Multitaper
        
        // Zero output power spectrum
        
        for (unsigned long j = 0; j < sizeOut; j++)
            output[j] = 0.0;
        
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
                
                r3 = (r1 - r2);
                i3 = (i1 - i2);
                
                output[j] += ((r3 * r3) + (i3 * i3)) * taperScale;
            }
        }
        
    }
    
    mAllocator->dealloc(tempMem);
}
