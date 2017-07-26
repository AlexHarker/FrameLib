
#ifndef FRAMELIB_FFT_H
#define FRAMELIB_FFT_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"

// FIX - add zero padding (why not do this prior to FFT with the pad object?)

class FrameLib_FFT : public FrameLib_Processor
{
	enum ParameterList {kMaxLength};
    
public:
	
    FrameLib_FFT(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 2)
    {
        mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
        mParameters.setMin(0);
        mParameters.setInstantiation();

        mParameters.set(serialisedParameters);
        
        unsigned long maxFFTSizeLog2 = log2(mParameters.getInt(kMaxLength));
		
		mFFTSetup = hisstools_create_setup_d(maxFFTSizeLog2);
        mMaxFFTSize = 1 << maxFFTSizeLog2;
    }
    
	~FrameLib_FFT()
	{
		hisstools_destroy_setup_d(mFFTSetup);
	}
	
private:
	
	unsigned long log2(unsigned long value)
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
	}
	
protected:
    
    void process()
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
	
protected:
	
	// FFT Setup
	
	FFT_SETUP_D mFFTSetup;
	
	// Maximum FFT Size
	
	unsigned long mMaxFFTSize;
};

#endif
