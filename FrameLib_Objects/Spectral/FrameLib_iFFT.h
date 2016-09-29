
#ifndef FRAMELIB_IFFT_H
#define FRAMELIB_IFFT_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"

// FIX - review FFTSetup

class FrameLib_iFFT : public FrameLib_Processor
{
	enum AttributeList {kMaxLength};
    
public:
	
    FrameLib_iFFT(FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(context, 2, 1)
    {
        mAttributes.addDouble(kMaxLength, "maxlength", 16384, 0);
        mAttributes.setMin(0.0);
        
        mAttributes.set(serialisedAttributes);
        
        unsigned long maxFFTSizeLog2 = log2(mAttributes.getInt(kMaxLength));
		
		mFFTSetup = hisstools_create_setup_d(maxFFTSizeLog2);
        mMaxFFTSize = 1 << maxFFTSizeLog2;
    }
    
	~FrameLib_iFFT()
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
	}
	
protected:
	
    void process ()
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
            FFTSizelog2 = log2((sizeIn - 1) << 1);
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
            double scale = 1.0 / (double) sizeOut;
            
            // Copy Spectrum
            
            for (unsigned long i = 0; (i < sizeIn) && (i < (sizeOut >> 1)); i++)
            {
                spectrum.realp[i] = input1[i];
                spectrum.imagp[i] = input2[i];
            }
            
            if (sizeIn == (sizeOut >> 1) + 1)
                spectrum.imagp[0] = input1[sizeOut >> 1];
            
            // Convert to time domain
         
            hisstools_rifft_d(mFFTSetup, &spectrum, FFTSizelog2);
            hisstools_zip_d(&spectrum, output, FFTSizelog2);
        
            // Scale
		
            for (unsigned long i = 0; i < sizeOut; i++)
                output[i] *= scale;
         
            mAllocator->dealloc(spectrum.realp);
        }
	}
	
	
protected:
	
	// FFT Setup
	
	FFT_SETUP_D mFFTSetup;
	
	// Maximum FFT Size
	
	unsigned long mMaxFFTSize;
};

#endif
