
#ifndef FRAMELIB_SPECTRALPROCESSOR_H
#define FRAMELIB_SPECTRALPROCESSOR_H

#include <algorithm>
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"
#include "../../FrameLib_Dependencies/HISSTools_IR_Manipulation/HISSTools_IR_Manipulation.h"

enum EdgeMode { kEdgeLinear, kEdgeWrap, kEdgeWrapCentre, kEdgeFold };

template <typename T> struct Types;

template<>
struct Types<float>
{
    using SplitType = FFT_SPLIT_COMPLEX_F;
    using SetupType = FFT_SETUP_F;
};

template<>
struct Types<double>
{
    using SplitType = FFT_SPLIT_COMPLEX_D;
    using SetupType = FFT_SETUP_D;
};


template <typename T, typename Allocator>
class Spectral_Processor
{
    using SplitType = typename Types<T>::SplitType;
    using SetupType = typename Types<T>::SetupType;
    
public:
    
    // Constructor
    
    Spectral_Processor(Allocator allocator) : mAllocator(allocator), mMaxFFTSizeLog2(0)
    {
        setMaxFFTSize(32768);
    }
    
    ~Spectral_Processor()
    {
        if (mMaxFFTSizeLog2)
            hisstools_destroy_setup(mFFTSetup);
    }
    
    void setMaxFFTSize(uintptr_t size)
    {
        uintptr_t FFTSizeLog2 = ilog2(size);
        
        if (FFTSizeLog2 != mMaxFFTSizeLog2)
        {
            if (mMaxFFTSizeLog2)
                hisstools_destroy_setup(mFFTSetup);
            if (FFTSizeLog2)
                hisstools_create_setup(&mFFTSetup, FFTSizeLog2);
            mMaxFFTSizeLog2 = FFTSizeLog2;
        }
    }
    
    uintptr_t maxFFTSize() const { return 1 << mMaxFFTSizeLog2; }
    
    // Transforms
    
    void fft(SplitType& io, uintptr_t FFTSizeLog2)
    {
        hisstools_fft(mFFTSetup, &io, FFTSizeLog2);
    }
    
    void rfft(SplitType& io, uintptr_t FFTSizeLog2)
    {
        hisstools_rfft(mFFTSetup, &io, FFTSizeLog2);
    }
    
    void rfft(SplitType& output, const T *input, uintptr_t size, uintptr_t FFTSizeLog2)
    {
        hisstools_rfft(mFFTSetup, input, &output, size, FFTSizeLog2);
    }
    
    void ifft(SplitType& io, uintptr_t FFTSizeLog2)
    {
        hisstools_ifft(mFFTSetup, &io, FFTSizeLog2);
    }
    
    void rifft(SplitType& io, uintptr_t FFTSizeLog2)
    {
        hisstools_rifft(mFFTSetup, &io, FFTSizeLog2);
    }
    
    void rifft(T *output, SplitType& input, uintptr_t FFTSizeLog2)
    {
        hisstools_rifft(mFFTSetup, &input, output, FFTSizeLog2);
    }
    
    // Convolution
    
    void convolve(T *rOut, T *iOut, const T *rIn1, uintptr_t sizeR1, const T *iIn1, uintptr_t sizeI1, const T *rIn2, uintptr_t sizeR2, const T *iIn2, uintptr_t sizeI2, EdgeMode mode)
    {
        binaryOp<&convolve_complex, &arrangeConvolve<SplitType>>(rOut, iOut, {rIn1, sizeR1}, {iIn1, sizeI1}, {rIn2, sizeR2}, {iIn2, sizeI2}, mode);
    }
    
    void convolveReal(T *output, const T *in1, uintptr_t size1, const T *in2, uintptr_t size2, EdgeMode mode)
    {
        binaryOp<&convolve_real, &arrangeConvolve<T*>>(output, {in1, size1}, {in2, size2}, mode);
    }
    
    // Correlation
    
    void correlate(T *rOut, T *iOut, const T *rIn1, uintptr_t sizeR1, const T *iIn1, uintptr_t sizeI1, const T *rIn2, uintptr_t sizeR2, const T *iIn2, uintptr_t sizeI2, EdgeMode mode)
    {
        binaryOp<&correlate_complex, &arrangeCorrelate<SplitType>>(rOut, iOut, {rIn1, sizeR1}, {iIn1, sizeI1}, {rIn2, sizeR2}, {iIn2, sizeI2}, mode);
    }
    
    void correlateReal(T *output, const T *in1, uintptr_t size1, const T *in2, uintptr_t size2, EdgeMode mode)
    {
        binaryOp<&correlate_real, &arrangeCorrelate<T*>>(output, {in1, size1}, {in2, size2}, mode);
    }
    
    // Phase
    
    void phase(T *output, const T *input, uintptr_t size, double phase)
    {
        uintptr_t FFTSizeLog2 = ilog2(size);
        uintptr_t FFTSize = 1 << FFTSizeLog2;
        
        TemporarySpectra<1> buffer(mAllocator, FFTSize >> 1);
        
        rfft(buffer.mSpectra[0], input, size, FFTSizeLog2);
        ir_phase(mFFTSetup, &buffer.mSpectra[0], &buffer.mSpectra[0], FFTSize, phase);
        rifft(output, buffer.mSpectra[0], FFTSizeLog2);
        
        scaleVector(output, FFTSize, T(0.5) / (T) FFTSize);
    }
    
    uintptr_t calcSize(uintptr_t size1, uintptr_t size2, EdgeMode mode)
    {
        Sizes sizes(size1, size2);
        
        if ((sizes.FFT() > maxFFTSize()) || !size1 || !size2)
            return 0;
        
        uintptr_t sizeOut = mode != kEdgeLinear ? sizes.max() : sizes.linear();
        
        if (mode == kEdgeFold && !(sizes.min() & 1U))
            return sizeOut + 1;
        
        return sizeOut;
    }
    
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
    
    // Scale Vector
    
    void scaleVector(T *io, uintptr_t size, T scale)
    {
        if (scale == 1.0)
            return;
        
        for (uintptr_t i = 0; i < size; i++)
            io[i] *= scale;
    }
    
    // Scale Spectrum
    
    void scaleSpectrum(SplitType &io, uintptr_t size, T scale)
    {
        if (scale == 1.0)
            return;
        
        for (uintptr_t i = 0; i < size; i++)
        {
            io.realp[i] *= scale;
            io.imagp[i] *= scale;
        }
    }
    
private:
    
    // Temporary Memory
    
    template <int N>
    struct TemporarySpectra
    {
        TemporarySpectra(Allocator& allocator, uintptr_t size) : mAllocator(allocator)
        {
            T* ptr = mAllocator.template allocate<T>(size * 2 * N * sizeof(T));
            
            for (int i = 0; i < N; i++)
            {
                mSpectra[i].realp = ptr + (N * 2 * size);
                mSpectra[i].imagp = ptr + ((N * 2 + 1) * size);
            }
        }
        
        ~TemporarySpectra()     { mAllocator.deallocate(mSpectra[0].realp); }
        
        TemporarySpectra(const TemporarySpectra&) = delete;
        TemporarySpectra & operator=(const TemporarySpectra&) = delete;
        
        operator bool()         { return mSpectra[0].realp; }
        
        Allocator &mAllocator;
        SplitType mSpectra[N];
    };
    
    struct Input
    {
        Input(const T* pointer, uintptr_t size) : mPointer(pointer), mSize(size) {}
        
        const T* mPointer;
        const uintptr_t mSize;
    };
    
    struct ZippedPointer
    {
        ZippedPointer(const SplitType spectrum, uintptr_t offset)
        : p1(spectrum.realp + (offset >> 1)), p2(spectrum.imagp + (offset >> 1))
        {
            if (offset & 1U)
                (*this)++;
        }
        
        const T *operator ++(int)
        {
            const T *p = p1;
            std::swap(++p1, p2);
            return p;
        }
        
        const T *operator --(int)
        {
            const T *p = p1;
            std::swap(p1, --p2);
            return p;
        }
        
    private:
        
        const T *p1, *p2;
    };
    
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
    
    typedef void SpectralOp(SplitType *, SplitType *, SplitType *, uintptr_t, T);
    typedef void ComplexArrange(SplitType, SplitType, const Sizes&, EdgeMode);
    typedef void RealArrange(T *, SplitType, const Sizes&, EdgeMode);

    // Memory manipulation (complex)
    
    static void copyZero(T* output, const T *input, uintptr_t inSize, uintptr_t outSize)
    {
        std::copy_n(input, inSize, output);
        std::fill_n(output + inSize, outSize - inSize, 0.0);
    }
    
    static void copy(SplitType& output, const SplitType& spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        std::copy_n(spectrum.realp + offset, size, output.realp + oOffset);
        std::copy_n(spectrum.imagp + offset, size, output.imagp + oOffset);
    }
    
    static void wrap(SplitType& output, const SplitType& spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        for (uintptr_t i = 0; i < size; i++)
        {
            output.realp[i + oOffset] += spectrum.realp[i + offset];
            output.imagp[i + oOffset] += spectrum.imagp[i + offset];
        }
    }
    
    static void fold(SplitType& output, const SplitType& spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        for (uintptr_t i = 0; i < size; i++)
        {
            output.realp[i + oOffset] += spectrum.realp[offset - (i + 1)];
            output.imagp[i + oOffset] += spectrum.imagp[offset - (i + 1)];
        }
    }

    // Memory manipulation (real)
    
    static void copy(T *output, const SplitType& spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        ZippedPointer p(spectrum, offset);

        for (uintptr_t i = 0; i < size; i++)
            output[oOffset + i] = *p++;
    }
    
    static void wrap(T *output, const SplitType& spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        ZippedPointer p(spectrum, offset);
        
        for (uintptr_t i = 0; i < size; i++)
            output[oOffset + i] += *p++;
    }
    
    static void fold(T *output, const SplitType& spectrum, uintptr_t oOffset, uintptr_t offset, uintptr_t size)
    {
        ZippedPointer p(spectrum, offset);
        
        for (uintptr_t i = 0; i < size; i++)
            output[oOffset + i] += *p--;
    }
    
    // Arranges for convolution and correlation
    
    template <class U>
    static void arrangeConvolve(U output, SplitType spectrum, const Sizes& sizes, EdgeMode mode)
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

    template <class U>
    static void arrangeCorrelate(U output, SplitType spectrum, const Sizes& sizes, EdgeMode mode)
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
   
    // Binary Operations
    
    template<SpectralOp Op, ComplexArrange arrange>
    void binaryOp(T *rOut, T *iOut, Input rIn1, Input iIn1, Input rIn2, Input iIn2, EdgeMode mode)
    {
        uintptr_t size1 = std::max(rIn1.mSize, iIn1.mSize);
        uintptr_t size2 = std::max(rIn2.mSize, iIn2.mSize);
        
        if (!calcSize(size1, size2, mode))
            return;
        
        // Special case for single sample inputs

        if (size1 == 1 && size2 == 1)
        {
            rOut[0] = rIn1.mPointer[0] * rIn2.mPointer[0] + iIn1.mPointer[0] * iIn2.mPointer[0];
            iOut[0] = rIn1.mPointer[0] * iIn2.mPointer[0] - iIn1.mPointer[0] * iIn1.mPointer[0];
            return;
        }
        
        // Assign temporary memory
        
        Sizes sizes(size1, size2);
        TemporarySpectra<2> buffers(mAllocator, sizes.FFT());
        SplitType output {rOut, iOut};
        
        // Process
        
        if (buffers)
        {
            copyZero(buffers.mSpectra[0].realp, rIn1.mPointer, rIn1.mSize, sizes.FFT());
            copyZero(buffers.mSpectra[0].imagp, iIn1.mPointer, iIn1.mSize, sizes.FFT());
            copyZero(buffers.mSpectra[1].realp, rIn2.mPointer, rIn2.mSize, sizes.FFT());
            copyZero(buffers.mSpectra[1].imagp, iIn2.mPointer, iIn2.mSize, sizes.FFT());
            
            fft(buffers.mSpectra[0], sizes.FFTLog2());
            fft(buffers.mSpectra[1], sizes.FFTLog2());
            
            Op(&buffers.mSpectra[0], &buffers.mSpectra[0], &buffers.mSpectra[1], sizes.FFT(), 1.0 / (T) sizes.FFT());
            
            ifft(buffers.mSpectra[0], sizes.FFTLog2());
            
            arrange(output, buffers.mSpectra[0], sizes, mode);
        }
    }
    
    template<SpectralOp Op, RealArrange arrange>
    void binaryOp(T *output, Input in1, Input in2, EdgeMode mode)
    {
        if (!calcSize(in1.mSize, in2.mSize, mode))
            return;
        
        // Special case for single sample inputs

        if (in1.mSize == 1 && in2.mSize == 1)
        {
            output[0] = in1.mPointer[0] * in2.mPointer[0];
            return;
        }
        
        // Assign temporary memory
        
        Sizes sizes(in1.mSize, in2.mSize);
        TemporarySpectra<2> buffers(mAllocator, sizes.FFT() >> 1);
        
        // Process
        
        if (buffers)
        {
            rfft(buffers.mSpectra[0], in1.mPointer, in1.mSize, sizes.FFTLog2());
            rfft(buffers.mSpectra[1], in2.mPointer, in2.mSize, sizes.FFTLog2());
            
            Op(&buffers.mSpectra[0], &buffers.mSpectra[0], &buffers.mSpectra[1], sizes.FFT() >> 1, 0.25 / (T) sizes.FFT());
            
            rifft(buffers.mSpectra[0], sizes.FFTLog2());
            
            arrange(output, buffers.mSpectra[0], sizes, mode);
        }
    }
    
    // Data
    
    Allocator mAllocator;
    SetupType mFFTSetup;
    uintptr_t mMaxFFTSizeLog2;
};

#endif
