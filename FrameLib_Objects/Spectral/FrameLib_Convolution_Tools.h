
#ifndef FRAMELIB_CONVOLUTION_TOOLS_H
#define FRAMELIB_CONVOLUTION_TOOLS_H

#include <algorithm>
#include "SIMDSupport.hpp"
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"
#include "FrameLib_Spectral_Functions.h"

enum EdgeMode { kEdgeLinear, kEdgeWrap, kEdgeWrapCentre, kEdgeFold };

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
    
    struct CorrelateOp
    {
        template<class T>
        void operator()(T &outR, T &outI, const T a, const T b, const T c, const T d, T scale)
        {
            outR = scale * (a * c + b * d);
            outI = scale * (b * c - a * d);
        }
    };
    
    struct ConvolveOp
    {
        template<class T>
        void operator()(T &outR, T &outI, const T a, const T b, const T c, const T d, T scale)
        {
            outR = scale * (a * c - b * d);
            outI = scale * (a * d + b * c);
        }
    };
    
    template<typename Op>
    void binaryOp(FFT_SPLIT_COMPLEX_D &io1, FFT_SPLIT_COMPLEX_D &in2, unsigned long dataLength, double scale, Op op)
    {        
        const int vec_size = SIMDLimits<double>::max_size;
        
        if (dataLength == 1 || dataLength < (vec_size / 2))
        {
            op(io1.realp[0], io1.imagp[0], io1.realp[0], io1.imagp[0], in2.realp[0], in2.imagp[0], scale);
        }
        else if (dataLength < vec_size)
        {
            const int current_vec_size = SIMDLimits<double>::max_size / 2;

            SIMDType<double, current_vec_size> *real1 = reinterpret_cast<SIMDType<double, current_vec_size> *>(io1.realp);
            SIMDType<double, current_vec_size> *imag1 = reinterpret_cast<SIMDType<double, current_vec_size> *>(io1.imagp);
            SIMDType<double, current_vec_size> *real2 = reinterpret_cast<SIMDType<double, current_vec_size> *>(in2.realp);
            SIMDType<double, current_vec_size> *imag2 = reinterpret_cast<SIMDType<double, current_vec_size> *>(in2.imagp);
            
            SIMDType<double, current_vec_size> scaleVec(scale);
            
            for (unsigned long i = 0; i < (dataLength / current_vec_size); i++)
                op(real1[i], imag1[i], real1[i], imag1[i], real2[i], imag2[i], scaleVec);
        }
        else
        {
            SIMDType<double, vec_size> *real1 = reinterpret_cast<SIMDType<double, vec_size> *>(io1.realp);
            SIMDType<double, vec_size> *imag1 = reinterpret_cast<SIMDType<double, vec_size> *>(io1.imagp);
            SIMDType<double, vec_size> *real2 = reinterpret_cast<SIMDType<double, vec_size> *>(in2.realp);
            SIMDType<double, vec_size> *imag2 = reinterpret_cast<SIMDType<double, vec_size> *>(in2.imagp);
            
            SIMDType<double, vec_size> scaleVec(scale);

            for (unsigned long i = 0; i < (dataLength / vec_size); i++)
                op(real1[i], imag1[i], real1[i], imag1[i], real2[i], imag2[i], scaleVec);
        }
    }
        
    template<typename Op>
    void binaryOpReal(FFT_SPLIT_COMPLEX_D &spectrum1, FFT_SPLIT_COMPLEX_D &spectrum2, unsigned long dataLength, double scale, Op op)
    {
        // Store DC and Nyquist Results
        
        const double DC = spectrum1.realp[0] * spectrum2.realp[0] * scale;
        const double Nyquist = spectrum1.imagp[0] * spectrum2.imagp[0] * scale;
        
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
    
    // Memory manipulation (complex)
    
    void copyZero(double *output, const double *input, unsigned long inSize, unsigned long outSize)
    {
        std::copy(input, input + inSize, output);
        std::fill_n(output + inSize, outSize - inSize, 0.0);
    }
    
    void copy(FFT_SPLIT_COMPLEX_D output, const FFT_SPLIT_COMPLEX_D spectrum, unsigned long outOffset, unsigned long offset, unsigned long size)
    {
        std::copy(spectrum.realp + offset, spectrum.realp + size + offset, output.realp + outOffset);
        std::copy(spectrum.imagp + offset, spectrum.imagp + size + offset, output.imagp + outOffset);
    }
    
    void wrap(FFT_SPLIT_COMPLEX_D output, const FFT_SPLIT_COMPLEX_D spectrum, unsigned long outOffset, unsigned long offset, unsigned long size)
    {
        for (unsigned long i = 0; i < size; i++)
        {
            output.realp[i + outOffset] += spectrum.realp[i + offset];
            output.imagp[i + outOffset] += spectrum.imagp[i + offset];
        }
    }
    
    void fold(FFT_SPLIT_COMPLEX_D output, const FFT_SPLIT_COMPLEX_D spectrum, unsigned long outOffset, unsigned long endOffset, unsigned long size)
    {
        for (unsigned long i = 1; i <= size; i++)
        {
            output.realp[i + outOffset - 1] += spectrum.realp[endOffset - i];
            output.imagp[i + outOffset - 1] += spectrum.imagp[endOffset - i];
        }
    }

    // Memory manipulation (real)

    struct assign { void operator()(double *output, const double *ptr) { *output = *ptr; } };
    struct accum { void operator()(double *output, const double *ptr) { *output += *ptr; } };
    struct increment { template<class T> T *operator()(T *&ptr) { return ptr++; } };
    struct decrement { template<class T> T *operator()(T *&ptr) { return ptr--; } };
    
    template <typename Op, typename PtrOp>
    void zip(double *output, const double *p1, const double *p2, unsigned long size, Op op, PtrOp pOp)
    {
        for (unsigned long i = 0; i < (size >> 1); i++)
        {
            op(pOp(output), pOp(p1));
            op(pOp(output), pOp(p2));
        }
        
        if (size & 1U)
            op(pOp(output), pOp(p1));
    }
    
    void copy(double *output, const FFT_SPLIT_COMPLEX_D spectrum, unsigned long outOffset, unsigned long offset, unsigned long size)
    {
        const double *p1 = (offset & 1U) ? spectrum.imagp + (offset >> 1) : spectrum.realp + (offset >> 1);
        const double *p2 = (offset & 1U) ? spectrum.realp + (offset >> 1) + 1 : spectrum.imagp + (offset >> 1);
        
        zip(output + outOffset, p1, p2, size, assign(), increment());
    }
    
    void wrap(double *output, const FFT_SPLIT_COMPLEX_D spectrum, unsigned long outOffset, unsigned long offset, unsigned long size)
    {
        const double *p1 = (offset & 1U) ? spectrum.imagp + (offset >> 1) : spectrum.realp + (offset >> 1);
        const double *p2 = (offset & 1U) ? spectrum.realp + (offset >> 1) + 1 : spectrum.imagp + (offset >> 1);
        
        zip(output + outOffset, p1, p2, size, accum(), increment());
    }
    
    void fold(double *output, const FFT_SPLIT_COMPLEX_D spectrum, unsigned long outOffset, unsigned long endOffset, unsigned long size)
    {
        const double *p1 = (endOffset & 1U) ? spectrum.realp + (endOffset >> 1) : spectrum.imagp + (endOffset >> 1) - 1;
        const double *p2 = (endOffset & 1U) ? spectrum.imagp + (endOffset >> 1) - 1 : spectrum.realp + (endOffset >> 1) - 1;
        
        zip(output + outOffset, p1, p2, size, accum(), decrement());
    }
    
    // Temporary Memory
    
    void tempSpectra(FFT_SPLIT_COMPLEX_D &spectrum1, FFT_SPLIT_COMPLEX_D &spectrum2, unsigned long dataSize)
    {
        spectrum1.realp = allocTemp(dataSize * 4 * sizeof(double));
        spectrum1.imagp = spectrum1.realp + dataSize;
        spectrum2.realp = spectrum1.imagp + dataSize;
        spectrum2.imagp = spectrum2.realp + dataSize;
    }
    
    unsigned long calcLinearSize(unsigned long size1, unsigned long size2)
    {
        return size1 + size2 - 1;
    }
    
    unsigned long calcSize(unsigned long size1, unsigned long size2, EdgeMode mode)
    {
        unsigned long linearSize = size1 + size2 - 1;
        unsigned long sizeOut = mode != kEdgeLinear ? std::max(size1, size2) : linearSize;
        unsigned long FFTSizelog2 = ilog2(linearSize);
        unsigned long FFTSize = 1 << FFTSizelog2;
        
        if (mode == kEdgeFold && !(std::min(size1, size2) & 1U))
            sizeOut++;
        
        if ((FFTSize > mMaxFFTSize) || !size1 || !size2)
            return 0;
        
        return sizeOut;
    }
    
    template <class T>
    void arrangeOutput(T output, FFT_SPLIT_COMPLEX_D spectrum, unsigned long minSize, unsigned long sizeOut, unsigned long linearSize, unsigned long FFTSize, EdgeMode mode, ConvolveOp op)
    {
        unsigned long offset = (mode == kEdgeFold || mode == kEdgeWrapCentre) ? (minSize - 1) / 2: 0;
        
        copy(output, spectrum, 0, offset, sizeOut);
        
        if (mode == kEdgeWrap)
            wrap(output, spectrum, 0, sizeOut, linearSize - sizeOut);
        
        if (mode == kEdgeWrapCentre)
        {
            unsigned long endWrap = (minSize - 1) - offset;
            
            wrap(output, spectrum, 0, linearSize - endWrap, endWrap);
            wrap(output, spectrum, sizeOut - offset, 0, offset);
        }
        
        if (mode == kEdgeFold)
        {
            unsigned long foldSize = minSize / 2;
            unsigned long foldOffset = sizeOut - foldSize;
            
            fold(output, spectrum, 0, foldSize, foldSize);
            fold(output, spectrum, foldOffset, linearSize, foldSize);
        }
    }

    template <class T>
    void arrangeOutput(T output, FFT_SPLIT_COMPLEX_D spectrum, unsigned long minSize, unsigned long sizeOut, unsigned long linearSize, unsigned long FFTSize, EdgeMode mode, CorrelateOp op)
    {
        unsigned long maxSize = (linearSize - minSize) + 1;

        if (mode == kEdgeLinear || mode == kEdgeWrap)
        {
            unsigned long extraSize = minSize - 1;
            
            copy(output, spectrum, 0, 0, maxSize);
            
            if (mode == kEdgeLinear)
                copy(output, spectrum, maxSize, FFTSize - extraSize, extraSize);
            else
                wrap(output, spectrum, (linearSize - (2 * (minSize - 1))), FFTSize - extraSize, extraSize);
        }
        else
        {
            unsigned long offset = minSize / 2;
            
            copy(output, spectrum, 0, FFTSize - offset, offset);
            copy(output, spectrum, offset, 0, sizeOut - offset);
            
            if (mode == kEdgeWrapCentre)
            {
                unsigned long endWrap = minSize - offset - 1;

                wrap(output, spectrum, 0, maxSize - offset, offset);
                wrap(output, spectrum, sizeOut - endWrap, FFTSize - (minSize - 1), endWrap);
            }
            else
            {
                fold(output, spectrum, 0, FFTSize - (offset - 1), offset);
                fold(output, spectrum, sizeOut - offset, maxSize, offset);
            }
        }
    }
   
    template<typename Op>
    void binarySpectralOperation(double *rOut, double *iOut, const double *rIn1, unsigned long sizeR1, const double *iIn1, unsigned long sizeI1,
                                 const double *rIn2, unsigned long sizeR2, const double *iIn2, unsigned long sizeI2, EdgeMode mode,  Op op)
    {
        FFT_SPLIT_COMPLEX_D spectrum1;
        FFT_SPLIT_COMPLEX_D spectrum2;
        unsigned long size1 = std::max(sizeR1, sizeI1);
        unsigned long size2 = std::max(sizeR2, sizeI2);
        
        unsigned long linearSize = calcLinearSize(size1, size2);
        unsigned long sizeOut = calcSize(size1, size2, mode);
        unsigned long FFTSizelog2 = ilog2(linearSize);
        unsigned long FFTSize = 1 << FFTSizelog2;
        
        // Special cases for short inputs
        
        if (!sizeOut)
            return;
            
        if (sizeOut == 1)
        {
            ConvolveOp()(rOut[0], iOut[0], rIn1[0], iIn1[0], rIn2[0], iIn2[0], 1.0);
            return;
        }
        
        // Assign temporary memory
        
        tempSpectra(spectrum1, spectrum2, FFTSize);
        
        if (spectrum1.realp)
        {
            FFT_SPLIT_COMPLEX_D output;
            output.realp = rOut;
            output.imagp = iOut;
            
            // Copy to the inputs
            
            copyZero(spectrum1.realp, rIn1, sizeR1, FFTSize);
            copyZero(spectrum1.imagp, iIn1, sizeI1, FFTSize);
            copyZero(spectrum2.realp, rIn2, sizeR2, FFTSize);
            copyZero(spectrum2.imagp, iIn2, sizeI2, FFTSize);
            
            binarySpectralOperation(spectrum1, spectrum2, FFTSizelog2, op);
            arrangeOutput(output, spectrum1, std::min(size1, size2), sizeOut, linearSize, FFTSize, mode, op);
        }
        
        // Deallocate
        
        deallocTemp(spectrum1.realp);
    }
    
    template<typename Op>
    void binarySpectralOperationReal(double *output, const double *in1, unsigned long size1, const double *in2, unsigned long size2, EdgeMode mode, Op op)
    {
        FFT_SPLIT_COMPLEX_D spectrum1;
        FFT_SPLIT_COMPLEX_D spectrum2;
        
        unsigned long linearSize = calcLinearSize(size1, size2);
        unsigned long sizeOut = calcSize(size1, size2, mode);
        unsigned long FFTSizelog2 = ilog2(linearSize);
        unsigned long FFTSize = 1 << FFTSizelog2;

        // Special cases for short inputs
        
        if (!sizeOut)
            return;
            
        if (sizeOut == 1)
        {
            output[0] = in1[0] * in2[0];
            return;
        }
        
        // Assign temporary memory
        
        tempSpectra(spectrum1, spectrum2, FFTSize >> 1);
        
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
            arrangeOutput(output, spectrum1, std::min(size1, size2), sizeOut, linearSize, FFTSize, mode, op);
        }
        
        // Deallocate
        
        deallocTemp(spectrum1.realp);
    }
    
    void convolveReal(double *output, const double *in1, unsigned long size1, const double *in2, unsigned long size2, EdgeMode mode)
    {
        binarySpectralOperationReal(output, in1, size1, in2, size2, mode, ConvolveOp());
    }
    
    void correlateReal(double *output, const double *in1, unsigned long size1, const double *in2, unsigned long size2, EdgeMode mode)
    {
        binarySpectralOperationReal(output, in1, size1, in2, size2, mode, CorrelateOp());
    }
    
    void convolve(double *rOut, double *iOut, const double *rIn1, unsigned long sizeR1, const double *iIn1, unsigned long sizeI1,
                  const double *rIn2, unsigned long sizeR2, const double *iIn2, unsigned long sizeI2, EdgeMode mode)
    {
        binarySpectralOperation(rOut, iOut, rIn1, sizeR1, iIn1, sizeI1, rIn2, sizeR2, iIn2, sizeI2, mode, ConvolveOp());
    }
    
    void correlate(double *rOut, double *iOut, const double *rIn1, unsigned long sizeR1, const double *iIn1, unsigned long sizeI1,
                  const double *rIn2, unsigned long sizeR2, const double *iIn2, unsigned long sizeI2, EdgeMode mode)
    {
        binarySpectralOperation(rOut, iOut, rIn1, sizeR1, iIn1, sizeI1, rIn2, sizeR2, iIn2, sizeI2, mode, CorrelateOp());
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


