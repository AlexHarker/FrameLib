
#ifndef FRAMELIB_SPECTRALPROCESSOR_H
#define FRAMELIB_SPECTRALPROCESSOR_H

#include <algorithm>
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"
#include "../../FrameLib_Dependencies/HISSTools_IR_Manipulation/HISSTools_IR_Manipulation.h"

enum EdgeMode { kEdgeLinear, kEdgeWrap, kEdgeWrapCentre, kEdgeFold };

class Spectral_Processor
{
    
public:
    
    // Constructor
    
    Spectral_Processor(FrameLib_Context context) : mAllocator(context)
    {
        uintptr_t maxLength = 32768;
        uintptr_t maxFFTSizeLog2 = ilog2(maxLength);
        hisstools_create_setup(&mFFTSetup, maxFFTSizeLog2);
        mMaxFFTSize = 1 << maxFFTSizeLog2;
    }
    
    ~Spectral_Processor()
    {
        hisstools_destroy_setup(mFFTSetup);
    }
    
    void setMaxFFTSize(uintptr_t maxLength)
    {
        if (maxLength != mMaxFFTSize)
        {
            hisstools_destroy_setup(mFFTSetup);
            uintptr_t maxFFTSizeLog2 = ilog2(maxLength);
            hisstools_create_setup(&mFFTSetup, maxFFTSizeLog2);
            mMaxFFTSize = 1 << maxFFTSizeLog2;
        }
    }
    
    // Scale Vector
    
    static uintptr_t ilog2(uintptr_t value)
    {
        uintptr_t bitShift = value;
        uintptr_t bitCount = 0;
        
        while (bitShift)
        {
            bitShift >>= 1U;
            bitCount++;
        }
        
        if (bitCount && value == 1U << (bitCount - 1U))
            return bitCount - 1U;
        else
            return bitCount;
    }
    
    void scaleVector(double *io, uintptr_t dataLength, double scale)
    {
        if (scale == 1.0)
            return;
        
        for (uintptr_t i = 0; i < dataLength; i++)
            io[i] *= scale;
    }
    
    // Scale Spectrum
    
    void scaleSpectrum(FFT_SPLIT_COMPLEX_D &io, uintptr_t dataLength, double scale)
    {
        if (scale == 1.0)
            return;
        
        for (uintptr_t i = 0; i < dataLength; i++)
        {
            io.realp[i] *= scale;
            io.imagp[i] *= scale;
        }
    }
    
    // Complex Forward Transform
    
    void transformForward(FFT_SPLIT_COMPLEX_D &io, uintptr_t FFTSizeLog2)
    {
        hisstools_fft(mFFTSetup, &io, FFTSizeLog2);
    }
    
    // Real Forward Transform (in-place)
    
    void transformForwardReal(FFT_SPLIT_COMPLEX_D &io, uintptr_t FFTSizeLog2)
    {
        hisstools_rfft(mFFTSetup, &io, FFTSizeLog2);
    }
    
    // Real Forward Transform (with unzipping)
    
    void transformForwardReal(FFT_SPLIT_COMPLEX_D &output, const double *input, uintptr_t size, uintptr_t FFTSizeLog2)
    {
        hisstools_rfft(mFFTSetup, input, &output, size, FFTSizeLog2);
    }
    
    // Complex Inverse Transform
    
    void transformInverse(FFT_SPLIT_COMPLEX_D &io, uintptr_t FFTSizeLog2)
    {
        hisstools_ifft(mFFTSetup, &io, FFTSizeLog2);
    }
    
    // Real Inverse Transform (in-place)
    
    void transformInverseReal(FFT_SPLIT_COMPLEX_D &io, uintptr_t FFTSizeLog2)
    {
        hisstools_rifft(mFFTSetup, &io, FFTSizeLog2);
    }
    
    // Real Inverse Transform (with zipping)
    
    void transformInverseReal(double *output, FFT_SPLIT_COMPLEX_D &input, uintptr_t FFTSizeLog2)
    {
        hisstools_rifft(mFFTSetup, &input, output, FFTSizeLog2);
    }
    
    void convolveReal(double *output, const double *in1, uintptr_t size1, const double *in2, uintptr_t size2, EdgeMode mode)
    {
        binaryOperationReal<&convolve_real, &arrangeConvolve<double*>>(output, in1, size1, in2, size2, mode);
    }
    
    void correlateReal(double *output, const double *in1, uintptr_t size1, const double *in2, uintptr_t size2, EdgeMode mode)
    {
        binaryOperationReal<&correlate_real, &arrangeCorrelate<double*>>(output, in1, size1, in2, size2, mode);
    }
    
    void convolve(double *rOut, double *iOut, const double *rIn1, uintptr_t sizeR1, const double *iIn1, uintptr_t sizeI1, const double *rIn2, uintptr_t sizeR2, const double *iIn2, uintptr_t sizeI2, EdgeMode mode)
    {
        binaryOperation<&convolve_complex, &arrangeConvolve<FFT_SPLIT_COMPLEX_D>>(rOut, iOut, rIn1, sizeR1, iIn1, sizeI1, rIn2, sizeR2, iIn2, sizeI2, mode);
    }
    
    void correlate(double *rOut, double *iOut, const double *rIn1, uintptr_t sizeR1, const double *iIn1, uintptr_t sizeI1, const double *rIn2, uintptr_t sizeR2, const double *iIn2, uintptr_t sizeI2, EdgeMode mode)
    {
        binaryOperation<&correlate_complex, &arrangeCorrelate<FFT_SPLIT_COMPLEX_D>>(rOut, iOut, rIn1, sizeR1, iIn1, sizeI1, rIn2, sizeR2, iIn2, sizeI2, mode);
    }
    
    uintptr_t calcSize(uintptr_t size1, uintptr_t size2, EdgeMode mode)
    {
        Sizes sizes(size1, size2);
        
        if ((sizes.FFT() > mMaxFFTSize) || !size1 || !size2)
            return 0;
        
        uintptr_t sizeOut = mode != kEdgeLinear ? sizes.max() : sizes.linear();
        
        if (mode == kEdgeFold && !(sizes.min() & 1U))
            return sizeOut + 1;
        
        return sizeOut;
    }
    
private:
    
    // Memory manipulation (complex)
    
    static void copyZero(double *output, const double *input, uintptr_t inSize, uintptr_t outSize)
    {
        std::copy_n(input, inSize, output);
        std::fill_n(output + inSize, outSize - inSize, 0.0);
    }
    
    static void copy(FFT_SPLIT_COMPLEX_D output, const FFT_SPLIT_COMPLEX_D spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        std::copy_n(spectrum.realp + offset, size, output.realp + oOffset);
        std::copy_n(spectrum.imagp + offset, size, output.imagp + oOffset);
    }
    
    static void wrap(FFT_SPLIT_COMPLEX_D output, const FFT_SPLIT_COMPLEX_D spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        for (uintptr_t i = 0; i < size; i++)
        {
            output.realp[i + oOffset] += spectrum.realp[i + offset];
            output.imagp[i + oOffset] += spectrum.imagp[i + offset];
        }
    }
    
    static void fold(FFT_SPLIT_COMPLEX_D output, const FFT_SPLIT_COMPLEX_D spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        for (uintptr_t i = 0; i < size; i++)
        {
            output.realp[i + oOffset] += spectrum.realp[offset - (i + 1)];
            output.imagp[i + oOffset] += spectrum.imagp[offset - (i + 1)];
        }
    }

    // Memory manipulation (real)
    
    struct zipPointer
    {
        zipPointer(const FFT_SPLIT_COMPLEX_D spectrum, uintptr_t offset)
        : p1(spectrum.realp + (offset >> 1)), p2(spectrum.imagp + (offset >> 1))
        {
            if (offset & 1U)
                (*this)++;
        }
        
        const double *operator ++(int)
        {
            const double *p = p1;
            std::swap(++p1, p2);
            return p;
        }
        
        const double *operator --(int)
        {
            const double *p = p1;
            std::swap(p1, --p2);
            return p;
        }
        
    private:
        
        const double *p1, *p2;
    };
    
    static void copy(double *output, const FFT_SPLIT_COMPLEX_D spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        zipPointer p(spectrum, offset);

        for (uintptr_t i = 0; i < size; i++)
            output[oOffset + i] = *p++;
    }
    
    static void wrap(double *output, const FFT_SPLIT_COMPLEX_D spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        zipPointer p(spectrum, offset);
        
        for (uintptr_t i = 0; i < size; i++)
            output[oOffset + i] += *p++;
    }
    
    static void fold(double *output, const FFT_SPLIT_COMPLEX_D spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        zipPointer p(spectrum, offset);
        
        for (uintptr_t i = 0; i < size; i++)
            output[oOffset + i] += *p--;
    }
    
    struct Sizes
    {
        Sizes(uintptr_t size1, uintptr_t size2)
        : mSize1(size1), mSize2(size2), mFFTSizeLog2(ilog2(linear())) {}
        
        uintptr_t linear() const        { return mSize1 + mSize2 - 1; }
        uintptr_t min() const           { return std::min(mSize1, mSize2); }
        uintptr_t max() const           { return std::max(mSize1, mSize2); }
        uintptr_t FFT() const           { return 1 << mFFTSizeLog2; }
        uintptr_t FFTLog2() const       { return mFFTSizeLog2; }

    private:
        uintptr_t mSize1, mSize2, mFFTSizeLog2;
    };
    
    template <class T>
    static void arrangeConvolve(T output, FFT_SPLIT_COMPLEX_D spectrum, const Sizes& sizes, EdgeMode mode)
    {
        uintptr_t folded = sizes.min() / 2;
        uintptr_t wrapped1 = (sizes.min() - 1) / 2;
        uintptr_t wrapped2 = (sizes.min() - 1) - wrapped1;
        
        switch (mode)
        {
            case kEdgeLinear:
                copy(output, spectrum, 0, 0, sizes.linear());
                break;

            case kEdgeWrap:
                copy(output, spectrum, 0, 0, sizes.max());
                wrap(output, spectrum, 0, sizes.max(), sizes.linear() - sizes.max());
                break;
        
            case kEdgeWrapCentre:
                copy(output, spectrum, 0, wrapped1, sizes.max());
                wrap(output, spectrum, 0, sizes.linear() - wrapped2, wrapped2);
                wrap(output, spectrum, sizes.max() - wrapped1, 0, wrapped1);
                break;
                
            case kEdgeFold:
                copy(output, spectrum, 0, (sizes.min() - 1) / 2, sizes.max());
                fold(output, spectrum, 0, folded, folded);
                fold(output, spectrum, sizes.max() - folded, sizes.linear(), folded);
                break;
        }
    }

    template <class T>
    static void arrangeCorrelate(T output, FFT_SPLIT_COMPLEX_D spectrum, const Sizes& sizes, EdgeMode mode)
    {
        uintptr_t offset = sizes.min() / 2;
        uintptr_t wrapped = sizes.min() - offset - 1;
        
        switch (mode)
        {
            case kEdgeLinear:
                copy(output, spectrum, 0, 0, sizes.max());
                copy(output, spectrum, sizes.max(), sizes.FFT() + 1 - sizes.min(), sizes.min() - 1);
                break;
                
            case kEdgeWrap:
                copy(output, spectrum, 0, 0, sizes.max());
                wrap(output, spectrum, sizes.linear() + 1 - (2 * sizes.min()), sizes.FFT() + 1 - sizes.min(), sizes.min() - 1);
                break;
                
            case kEdgeWrapCentre:
                copy(output, spectrum, 0, sizes.FFT() - offset, offset);
                copy(output, spectrum, offset, 0, sizes.max() - offset);
                wrap(output, spectrum, 0, sizes.max() - offset, offset);
                wrap(output, spectrum, sizes.max() - wrapped, sizes.FFT() + 1 - sizes.min(), wrapped);
                break;
            
            case kEdgeFold:
                copy(output, spectrum, 0, sizes.FFT() - offset, offset);
                copy(output, spectrum, offset, 0, sizes.max() - offset);
                fold(output, spectrum, 0, sizes.FFT() + 1 - offset, offset);
                fold(output, spectrum, sizes.max() - offset, sizes.max(), offset);
                break;
        }
    }
   
    typedef void SpectralOp(FFT_SPLIT_COMPLEX_D *, FFT_SPLIT_COMPLEX_D *, FFT_SPLIT_COMPLEX_D *, uintptr_t, double);
    typedef void ComplexArrange(FFT_SPLIT_COMPLEX_D, FFT_SPLIT_COMPLEX_D, const Sizes&, EdgeMode);
    typedef void RealArrange(double *, FFT_SPLIT_COMPLEX_D, const Sizes&, EdgeMode);

    // Temporary Memory
    
    void tempSpectra(FFT_SPLIT_COMPLEX_D &spectrum1, FFT_SPLIT_COMPLEX_D &spectrum2, uintptr_t dataSize)
    {
        spectrum1.realp = allocTemp(dataSize * 4 * sizeof(double));
        spectrum1.imagp = spectrum1.realp + dataSize;
        spectrum2.realp = spectrum1.imagp + dataSize;
        spectrum2.imagp = spectrum2.realp + dataSize;
    }
    
    template<SpectralOp Op, ComplexArrange arrange>
    void binaryOperation(double *rOut, double *iOut, const double *rIn1, uintptr_t sizeR1, const double *iIn1, uintptr_t sizeI1, const double *rIn2, uintptr_t sizeR2, const double *iIn2, uintptr_t sizeI2, EdgeMode mode)
    {
        uintptr_t size1 = std::max(sizeR1, sizeI1);
        uintptr_t size2 = std::max(sizeR2, sizeI2);
        
        if (!calcSize(size1, size2, mode))
            return;
        
        // Special case for single sample inputs

        if (size1 == 1 && size2 == 1)
        {
            rOut[0] = rIn1[0] * rIn2[0] + iIn1[0] * iIn2[0];
            iOut[0] = rIn1[0] * iIn2[0] - iIn1[0] * iIn1[0];
            return;
        }
        
        // Assign temporary memory
        
        FFT_SPLIT_COMPLEX_D spectrum1;
        FFT_SPLIT_COMPLEX_D spectrum2;
        Sizes sizes(size1, size2);
        tempSpectra(spectrum1, spectrum2, sizes.FFT());
        
        if (spectrum1.realp)
        {
            FFT_SPLIT_COMPLEX_D output;
            output.realp = rOut;
            output.imagp = iOut;
            
            copyZero(spectrum1.realp, rIn1, sizeR1, sizes.FFT());
            copyZero(spectrum1.imagp, iIn1, sizeI1, sizes.FFT());
            copyZero(spectrum2.realp, rIn2, sizeR2, sizes.FFT());
            copyZero(spectrum2.imagp, iIn2, sizeI2, sizes.FFT());
            
            transformForward(spectrum1, sizes.FFTLog2());
            transformForward(spectrum2, sizes.FFTLog2());
            
            Op(&spectrum1, &spectrum1, &spectrum2, sizes.FFT(), 1.0 / (double) sizes.FFT());
            
            transformInverse(spectrum1, sizes.FFTLog2());
            
            arrange(output, spectrum1, sizes, mode);
        }
        
        // Deallocate
        
        deallocTemp(spectrum1.realp);
    }
    
    template<SpectralOp Op, RealArrange arrange>
    void binaryOperationReal(double *output, const double *in1, uintptr_t size1, const double *in2, uintptr_t size2, EdgeMode mode)
    {
        if (!calcSize(size1, size2, mode))
            return;
        
        // Special case for single sample inputs

        if (size1 == 1 && size2 == 1)
        {
            output[0] = in1[0] * in2[0];
            return;
        }
        
        // Assign temporary memory
        
        FFT_SPLIT_COMPLEX_D spectrum1;
        FFT_SPLIT_COMPLEX_D spectrum2;
        Sizes sizes(size1, size2);
       
        tempSpectra(spectrum1, spectrum2, sizes.FFT() >> 1);
        
        if (spectrum1.realp)
        {
            transformForwardReal(spectrum1, in1, size1, sizes.FFTLog2());
            transformForwardReal(spectrum2, in2, size2, sizes.FFTLog2());
            
            Op(&spectrum1, &spectrum1, &spectrum2, sizes.FFT() >> 1, 0.25 / (double) sizes.FFT());
            
            transformInverseReal(spectrum1, sizes.FFTLog2());
            
            arrange(output, spectrum1, sizes, mode);
        }
        
        // Deallocate
        
        deallocTemp(spectrum1.realp);
    }
    
    double *allocTemp(uintptr_t N)
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
    
    uintptr_t mMaxFFTSize;
};

#endif
