
#ifndef FRAMELIB_CONVOLUTION_TOOLS_H
#define FRAMELIB_CONVOLUTION_TOOLS_H

#include <algorithm>
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"
#include "FrameLib_Spectral_Functions.h"

class Spectral
{
    
public:
    
    // Constructor
    
    Spectral(FrameLib_Context context) : mAllocator(context)
    {
        unsigned long maxLength = 32768;
        unsigned long maxFFTSizeLog2 = ilog2(maxLength);
        hisstools_create_setup(&mFFTSetup, maxFFTSizeLog2);
        mMaxFFTSize = 1 << maxFFTSizeLog2;
    }
    
    ~Spectral()
    {
        hisstools_destroy_setup(mFFTSetup);
    }
    
    void setMaxFFTSize(unsigned long maxLength)
    {
        if (maxLength != mMaxFFTSize)
        {
            hisstools_destroy_setup(mFFTSetup);
            unsigned long maxFFTSizeLog2 = ilog2(maxLength);
            hisstools_create_setup(&mFFTSetup, maxFFTSizeLog2);
            mMaxFFTSize = 1 << maxFFTSizeLog2;
        }
    }
    
    // Scale Vector
    
    void scaleVector(double *io, unsigned long dataLength, double scale)
    {
        if (scale == 1.0)
            return;
        
        for (unsigned long i = 0; i < dataLength; i++)
            io[i] *= scale;
    }
    
    // Scale Spectrum
    
    void scaleSpectrum(FFT_SPLIT_COMPLEX_D &io, unsigned long dataLength, double scale)
    {
        if (scale == 1.0)
            return;
        
        for (unsigned long i = 0; i < dataLength; i++)
        {
            io.realp[i] *= scale;
            io.imagp[i] *= scale;
        }
    }
    
    // Complex Forward Transform
    
    void transformForward(FFT_SPLIT_COMPLEX_D &io, unsigned long FFTSizelog2)
    {
        hisstools_fft(mFFTSetup, &io, FFTSizelog2);
    }
    
    // Real Forward Transform (in-place)
    
    void transformForwardReal(FFT_SPLIT_COMPLEX_D &io, unsigned long FFTSizelog2)
    {
        hisstools_rfft(mFFTSetup, &io, FFTSizelog2);
    }
    
    // Real Forward Tansform (with unzipping)
    
    void transformForwardReal(FFT_SPLIT_COMPLEX_D &output, const double *input, unsigned long size, unsigned long FFTSizelog2)
    {
        hisstools_rfft(mFFTSetup, input, &output, size, FFTSizelog2);
    }
    
    // Complex Inverse Transform
    
    void transformInverse(FFT_SPLIT_COMPLEX_D &io, unsigned long FFTSizelog2)
    {
        hisstools_ifft(mFFTSetup, &io, FFTSizelog2);
    }
    
    // Real Inverse Transform (in-place)
    
    void transformInverseReal(FFT_SPLIT_COMPLEX_D &io, unsigned long FFTSizelog2)
    {
        hisstools_rifft(mFFTSetup, &io, FFTSizelog2);
    }
    
    // Real Inverse Transform (with zipping)
    
    void transformInverseReal(double *output, FFT_SPLIT_COMPLEX_D &input, unsigned long FFTSizelog2)
    {
        hisstools_rifft(mFFTSetup, &input, output, FFTSizelog2);
    }
    
    bool checkSize(unsigned long size)
    {
        unsigned long FFTSizelog2 = ilog2(size);
        unsigned long FFTSize = 1 << FFTSizelog2;
        
        return FFTSize <= mMaxFFTSize;
    }
    
    struct CorrelateOp
    {
        void operator()(double &outR, double &outI, const double a, const double b, const double c, const double d, double scale)
        {
            outR = scale * (a * c + b * d);
            outI = scale * (b * c - a * d);
        }
      
        double operator()(const double r1, const double r2, double scale) { return scale * (r1 * r2); }
    };
    
    struct ConvolveOp
    {
        void operator()(double &outR, double &outI, const double a, const double b, const double c, const double d, double scale)
        {
            outR = scale * (a * c - b * d);
            outI = scale * (a * d + b * c);
        }
        
        double operator()(const double r1, const double r2, double scale) { return scale * (r1 * r2); }
    };
    
    template<typename Op>
    void binaryOp(FFT_SPLIT_COMPLEX_D &io1, FFT_SPLIT_COMPLEX_D &in2, unsigned long dataLength, double scale, Op op)
    {
        // FIX - vectorise here (with appropriate changes to ops)
        
        for (unsigned long i = 0; i < dataLength; i++)
            op(io1.realp[i], io1.imagp[i], io1.realp[i], io1.imagp[i], in2.realp[i], in2.imagp[i], scale);
    }
        
    template<typename Op>
    void binaryOpReal(FFT_SPLIT_COMPLEX_D &spectrum1, FFT_SPLIT_COMPLEX_D &spectrum2, unsigned long dataLength, double scale, Op op)
    {
        // Store DC and Nyquist
        
        const double DC = op(spectrum1.realp[0], spectrum2.realp[0], scale);
        const double Nyquist = op(spectrum1.imagp[0], spectrum2.imagp[0], scale);
        
        binaryOp(spectrum1, spectrum2, dataLength, scale, op);
        
        // Set DC and Nyquist bins
        
        spectrum1.realp[0] = DC;
        spectrum1.imagp[0] = Nyquist;
    }
    
    template<typename Op>
    void binarySpectralOperation(FFT_SPLIT_COMPLEX_D &io1, FFT_SPLIT_COMPLEX_D &in2, unsigned long FFTSizelog2, Op op)
    {
        unsigned long FFTSize = 1 << FFTSizelog2;
        
        // Take the Forward FFTs
        
        transformForward(io1, FFTSizelog2);
        transformForward(in2, FFTSizelog2);
        
        // Operate
        
        double scale = 1.0 / (double) FFTSize;
        binaryOp(io1, in2, FFTSize, scale, Op());
        
        // Inverse iFFT
        
        transformInverse(io1, FFTSizelog2);
    }
    
    void copy(double *output, const double *input, unsigned long inSize, unsigned long outSize)
    {
        std::copy(input, input + inSize, output);
        std::fill_n(output + inSize, outSize - inSize, 0.0);
    }
    
    template<typename Op>
    void binarySpectralOperation(double *rOut, double *iOut, const double *rIn1, unsigned long sizeR1, const double *iIn1, unsigned long sizeI1,
                                 const double *rIn2, unsigned long sizeR2, const double *iIn2, unsigned long sizeI2, Op op)
    {
        FFT_SPLIT_COMPLEX_D spectrum1;
        FFT_SPLIT_COMPLEX_D spectrum2;
        unsigned long size1 = std::max(sizeR1, sizeI1);
        unsigned long size2 = std::max(sizeR2, sizeI2);
        unsigned long sizeOut = size1 + size2 - 1;
        unsigned long FFTSizelog2 = ilog2(sizeOut);
        unsigned long FFTSize = 1 << FFTSizelog2;
        
        // Special case for short inputs
        
        if (sizeOut < 2)
        {
            if (size1 == 1 && size2 == 1)
            {
                // FIX THIS
                //output[0] = in1[0] * in2[0];
            }
            return;
        }
        
        // Assign temporary memory
        
        spectrum1.realp = allocTemp(FFTSize * 4 * sizeof(double));
        spectrum1.imagp = spectrum1.realp + FFTSize;
        spectrum2.realp = spectrum1.imagp + FFTSize;
        spectrum2.imagp = spectrum2.realp + FFTSize;
        
        if (spectrum1.realp)
        {
            // Copy to the inputs
            
            copy(spectrum1.realp, rIn1, sizeR1, FFTSize);
            copy(spectrum1.imagp, iIn1, sizeI1, FFTSize);
            copy(spectrum2.realp, rIn2, sizeR2, FFTSize);
            copy(spectrum2.imagp, iIn2, sizeI2, FFTSize);
            
            binarySpectralOperation(spectrum1, spectrum2, FFTSizelog2, op);
            
            // Copy to the output
            
            std::copy(spectrum1.realp, spectrum1.realp + sizeOut, rOut);
            std::copy(spectrum2.realp, spectrum2.realp + sizeOut, iOut);
        }
        
        // Deallocate
        
        deallocTemp(spectrum1.realp);
    }
    
    template<typename Op>
    void binarySpectralOperationReal(double *output, const double *in1, unsigned long size1, const double *in2, unsigned long size2, Op op)
    {
        FFT_SPLIT_COMPLEX_D spectrum1;
        FFT_SPLIT_COMPLEX_D spectrum2;
        unsigned long sizeOut = size1 + size2 - 1;
        unsigned long FFTSizelog2 = ilog2(sizeOut);
        unsigned long FFTSize = 1 << FFTSizelog2;

        // Special case for short inputs
        
        if (sizeOut < 2)
        {
            if (size1 == 1 && size2 == 1)
                output[0] = in1[0] * in2[0];
            return;
        }
        
        // Assign temporary memory
        
        spectrum1.realp = allocTemp(FFTSize * 2 * sizeof(double));
        spectrum1.imagp = spectrum1.realp + (FFTSize >> 1);
        spectrum2.realp = spectrum1.imagp + (FFTSize >> 1);
        spectrum2.imagp = spectrum2.realp + (FFTSize >> 1);
        
        if (spectrum1.realp)
        {
            // Take the Forward Real FFTs
            
            transformForwardReal(spectrum1, in1, size1, FFTSizelog2);
            transformForwardReal(spectrum2, in2, size2, FFTSizelog2);
            
            // Operate
            
            double scale = 0.25 / (double) FFTSize;
            binaryOpReal(spectrum1, spectrum2, FFTSize >> 1, scale, Op());
            
            // Inverse iFFT
            
            transformInverseReal(spectrum1, FFTSizelog2);
           
            // Copy to the output with zipping
            
            for (unsigned long i = 0; i < (sizeOut >> 1); i++)
            {
                *output++ = spectrum1.realp[i];
                *output++ = spectrum1.imagp[i];
            }
            
            if (sizeOut | 1U)
                *output++ = spectrum1.realp[sizeOut >> 1];
        }
        
        // Deallocate
        
        deallocTemp(spectrum1.realp);
    }
    
    void convolveReal(double *output, const double *in1, unsigned long size1, const double *in2, unsigned long size2)
    {
        binarySpectralOperationReal(output, in1, size1, in2, size2, ConvolveOp());
    }
    
    void correlateReal(double *output, const double *in1, unsigned long size1, const double *in2, unsigned long size2)
    {
        binarySpectralOperationReal(output, in1, size1, in2, size2, CorrelateOp());
    }
    
    void convolve(double *rOut, double *iOut, const double *rIn1, unsigned long sizeR1, const double *iIn1, unsigned long sizeI1,
                  const double *rIn2, unsigned long sizeR2, const double *iIn2, unsigned long sizeI2)
    {
        binarySpectralOperation(rOut, iOut, rIn1, sizeR1, iIn1, sizeI1, rIn2, sizeR2, iIn2, sizeI2, ConvolveOp());
    }
    
    void correlate(double *rOut, double *iOut, const double *rIn1, unsigned long sizeR1, const double *iIn1, unsigned long sizeI1,
                  const double *rIn2, unsigned long sizeR2, const double *iIn2, unsigned long sizeI2)
    {
        binarySpectralOperation(rOut, iOut, rIn1, sizeR1, iIn1, sizeI1, rIn2, sizeR2, iIn2, sizeI2, CorrelateOp());
    }

private:
    
    double *allocTemp(unsigned long N)
    {
        return reinterpret_cast<double *>(mAllocator->alloc(sizeof(double) * N));
    }
    
    void deallocTemp(double *& ptr)
    {
        mAllocator->dealloc(ptr);
    }
    
    // Allocator
    
    FrameLib_Context::Allocator mAllocator;
    
    // FFT Setup
    
    FFT_SETUP_D mFFTSetup;
    
    // Maximum FFT Size
    
    unsigned long mMaxFFTSize;
};

#endif


