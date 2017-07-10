
#ifndef FRAMELIB_CONVOLVE_H
#define FRAMELIB_CONVOLVE_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"


class FrameLib_Convolve : public FrameLib_Processor
{
	enum ParameterList {kMaxLength};
    
public:
	
    FrameLib_Convolve(FrameLib_Context context, FrameLib_Parameters::SerialBase *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
    {
        mParameters.addDouble(kMaxLength, "maxlength", 16384, 0);
        mParameters.setMin(0.0);
        
        mParameters.set(serialisedParameters);
        
        unsigned long maxFFTSizeLog2 = log2(mParameters.getInt(kMaxLength));
		
		mFFTSetup = hisstools_create_setup_d(maxFFTSizeLog2);
        mMaxFFTSize = 1 << maxFFTSizeLog2;
    }
    
	~FrameLib_Convolve()
	{
		hisstools_destroy_setup_d(mFFTSetup);
	}
	
private:
	
	unsigned long log2 (unsigned long value)
	{
		unsigned long bitShift = value;
		unsigned long bitCount = 0;
		
		while (bitShift)
		{
			bitShift >>= 1U;
			bitCount++;
		}
		
		if (value == 1U << (bitCount - 1U))
			return bitCount - 1U;
		else
			return bitCount;
	};
	
protected:
    
    void process ()
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
	
protected:
	
	// FFT Setup
	
	FFT_SETUP_D mFFTSetup;
	
	// Maximum FFT Size
	
	unsigned long mMaxFFTSize;
};

#endif
