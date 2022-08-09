
#ifndef SPECTRALPROCESSOR_H
#define SPECTRALPROCESSOR_H

#include <algorithm>

#include "Allocator.hpp"
#include "HISSTools_FFT/HISSTools_FFT.h"
#include "SpectralFunctions.hpp"
#include <type_traits>

template <typename T, typename Allocator = aligned_allocator>
class spectral_processor
{
    using Split = typename FFTTypes<T>::Split;
    using Setup = typename FFTTypes<T>::Setup;
    
    template <bool B>
    using enable_if_t = typename std::enable_if<B, int>::type;
    
public:
    
    enum class EdgeMode { Linear, Wrap, WrapCentre, Fold, FoldRepeat };
    
    struct in_ptr
    {
        in_ptr(const T* ptr, uintptr_t size) : m_ptr(ptr), m_size(size) {}
        
        const T* m_ptr;
        const uintptr_t m_size;
    };
    
    // Constructor
    
    template <typename U = Allocator, enable_if_t<std::is_default_constructible<U>::value> = 0>
    spectral_processor(uintptr_t max_fft_size = 32768)
    : m_max_fft_size_log2(0)
    , m_fft_setup(nullptr)
    {
        if (max_fft_size)
            set_max_fft_size(max_fft_size);
    }
    
    template <typename U = Allocator, enable_if_t<std::is_copy_constructible<U>::value> = 0>
    spectral_processor(const Allocator& allocator, uintptr_t max_fft_size = 32768)
    : m_allocator(allocator)
    , m_fft_setup(nullptr)
    , m_max_fft_size_log2(0)
    {
        if (max_fft_size)
            set_max_fft_size(max_fft_size);
    }
    
    template <typename U = Allocator, enable_if_t<std::is_move_constructible<U>::value> = 0>
    spectral_processor(Allocator&& allocator, uintptr_t max_fft_size = 32768)
    : m_allocator(allocator)
    , m_fft_setup(nullptr)
    , m_max_fft_size_log2(0)
    {
        if (max_fft_size)
            set_max_fft_size(0);
    }
    
    // Not Copyable
    
    spectral_processor(const spectral_processor&) = delete;
    spectral_processor &operator =(const spectral_processor&) = delete;
    
    // Moveable (subject to the allocator being moveable)
    
    template <typename U = Allocator, enable_if_t<std::is_move_constructible<U>::value> = 0>
    spectral_processor(spectral_processor&& b)
    : m_allocator(std::move(b.m_allocator))
    , m_fft_setup(std::move(b.m_fft_setup))
    , m_max_fft_size_log2(b.m_max_fft_size_log2)
    {
        b.m_fft_setup = nullptr;
    }
    
    template <typename U = Allocator, enable_if_t<std::is_move_assignable<U>::value> = 0>
    spectral_processor &operator =(spectral_processor&& b)
    {
        m_allocator = std::move(b.m_allocator);
        m_fft_setup = std::move(b.m_fft_setup);
        m_max_fft_size_log2 = b.m_max_fft_size_log2;
        b.m_fft_setup = nullptr;
        
        return *this;
    }
    
    // Destructor
    
    ~spectral_processor()
    {
        if (m_max_fft_size_log2)
            hisstools_destroy_setup(m_fft_setup);
    }
    
    void set_max_fft_size(uintptr_t size)
    {
        uintptr_t fft_size_log2 = calc_fft_size_log2(size);
        
        if (fft_size_log2 != m_max_fft_size_log2)
        {
            if (m_max_fft_size_log2)
                hisstools_destroy_setup(m_fft_setup);
            if (fft_size_log2)
                hisstools_create_setup(&m_fft_setup, fft_size_log2);
            m_max_fft_size_log2 = fft_size_log2;
        }
    }
    
    uintptr_t max_fft_size() const { return uintptr_t(1) << m_max_fft_size_log2; }
    
    // Transforms
    
    void fft(Split& io, uintptr_t fft_size_log2)
    {
        if (fft_size_log2)
            hisstools_fft(m_fft_setup, &io, fft_size_log2);
    }
    
    void rfft(Split& io, uintptr_t fft_size_log2)
    {
        if (!fft_size_log2)
            io[0] * T(2);
        else
            hisstools_rfft(m_fft_setup, &io, fft_size_log2);
    }
    
    void rfft(Split& output, const T *input, uintptr_t size, uintptr_t fft_size_log2)
    {
        if (!fft_size_log2)
        {
            output.realp[0] = input[0] * T(2);
            output.imagp[0] = T(0);
        }
        else
            hisstools_rfft(m_fft_setup, input, &output, size, fft_size_log2);
    }
    
    void ifft(Split& io, uintptr_t fft_size_log2)
    {
        if (fft_size_log2)
            hisstools_ifft(m_fft_setup, &io, fft_size_log2);
    }
    
    void rifft(Split& io, uintptr_t fft_size_log2)
    {
        if (fft_size_log2)
            hisstools_rifft(m_fft_setup, &io, fft_size_log2);
    }
    
    void rifft(T *output, Split& input, uintptr_t fft_size_log2)
    {
        if (!fft_size_log2)
            output[0] = input.realp[0];
        else
            hisstools_rifft(m_fft_setup, &input, output, fft_size_log2);
    }
    
    // Convolution
    
    void convolve(T *r_out, T *i_out, in_ptr r_in1, in_ptr i_in1, in_ptr r_in2, in_ptr i_in2, EdgeMode mode)
    {
        binary_op<ir_convolve_complex, arrange_convolve<Split>>(r_out, i_out, r_in1, i_in1, r_in2, i_in2, mode);
    }
    
    void convolve(T *output, in_ptr in1, in_ptr in2, EdgeMode mode)
    {
        binary_op<ir_convolve_real, arrange_convolve<T*>>(output, in1, in2, mode);
    }
    
    // Correlation
    
    void correlate(T *r_out, T *i_out, in_ptr r_in1, in_ptr i_in1, in_ptr r_in2, in_ptr i_in2, EdgeMode mode)
    {
        binary_op<ir_correlate_complex, arrange_correlate<Split>>(r_out, i_out, r_in1, i_in1, r_in2, i_in2, mode);
    }
    
    void correlate(T *output, in_ptr in1, in_ptr in2, EdgeMode mode)
    {
        binary_op<ir_correlate_real, arrange_correlate<T*>>(output, in1, in2, mode);
    }
    
    // Phase
    
    void change_phase(T *output, const T *input, uintptr_t size, double phase, double time_multiplier = 1.0)
    {
        uintptr_t fft_size_log2 = calc_fft_size_log2((uintptr_t) std::round(size * time_multiplier));
        uintptr_t fft_size = uintptr_t(1) << fft_size_log2;
        
        // Special case for a single sample input
        
        if (size == 1)
        {
            output[0] = input[0];
            return;
        }
        
        temporary_buffers<1> buffer(m_allocator, fft_size >> 1);
        
        rfft(buffer.m_spectra[0], input, size, fft_size_log2);
        ir_phase(m_fft_setup, &buffer.m_spectra[0], &buffer.m_spectra[0], fft_size, phase);
        rifft(output, buffer.m_spectra[0], fft_size_log2);
        
        scale_vector(output, fft_size, T(0.5) / (T) fft_size);
    }
    
    uintptr_t convolved_size(uintptr_t size1, uintptr_t size2, EdgeMode mode) const
    {
        return calc_conv_corr_size(size1, size2, mode);
    }
    
    uintptr_t correlated_size(uintptr_t size1, uintptr_t size2, EdgeMode mode) const
    {
        return calc_conv_corr_size(size1, size2, mode);
    }
    
    uintptr_t required_fft_size(uintptr_t size1, uintptr_t size2) const
    {
        if (!size1 || !size2)
            return 0;
        
        op_sizes sizes(size1, size2, EdgeMode::Linear);
        
        return sizes.fft();
    }
    
    static uintptr_t calc_fft_size_log2(uintptr_t size)
    {
        uintptr_t count = 0;
        
        while (size >> count)
            count++;
        
        if (count && size == uintptr_t(1) << (count - 1U))
            return count - uintptr_t(1);
        else
            return count;
    }
    
    // Scale Vector
    
    void scale_vector(T *io, uintptr_t size, T scale)
    {
        if (scale == 1.0)
            return;
        
        for (uintptr_t i = 0; i < size; i++)
            io[i] *= scale;
    }
    
    // Scale Spectrum
    
    void scale_spectrum(Split &io, uintptr_t size, T scale)
    {
        if (scale == 1.0)
            return;
        
        for (uintptr_t i = 0; i < size; i++)
        {
            io.realp[i] *= scale;
            io.imagp[i] *= scale;
        }
    }
    
protected:
    
    // Temporary Memory
    
    template <int N>
    struct temporary_buffers
    {
        temporary_buffers(Allocator& allocator, uintptr_t size) : m_allocator(allocator)
        {
            T* ptr = m_allocator.template allocate<T>(size * 2 * N * sizeof(T));
            
            for (int i = 0; i < N; i++)
            {
                m_spectra[i].realp = ptr + (i * 2 * size);
                m_spectra[i].imagp = ptr + ((i * 2 + 1) * size);
            }
        }
        
        temporary_buffers(const temporary_buffers&) = delete;
        temporary_buffers & operator=(const temporary_buffers&) = delete;
        
        ~temporary_buffers() { m_allocator.deallocate(m_spectra[0].realp); }
        
        operator bool() { return m_spectra[0].realp; }
        
        Allocator &m_allocator;
        Split m_spectra[N];
    };
    
    struct zipped_pointer
    {
        zipped_pointer(const Split spectrum, uintptr_t offset)
        : p1(spectrum.realp + (offset >> 1)), p2(spectrum.imagp + (offset >> 1))
        {
            if (offset & 1U)
                (*this)++;
        }
        
        const T *operator ++()
        {
            std::swap(++p1, p2);
            return p1;
        }
        
        const T *operator ++(int)
        {
            std::swap(p1, p2);
            return p2++;
        }
        
    private:
        
        const T *p1, *p2;
    };
    
    struct op_sizes
    {
        op_sizes(uintptr_t size1, uintptr_t size2, EdgeMode mode)
        : m_mode(mode), m_size1(size1), m_size2(size2), m_fft_size_log2(calc_fft_size_log2(calc_size()))
        {}
        
        EdgeMode mode() const           { return m_mode; }
        
        bool foldMode() const           { return m_mode == EdgeMode::Fold || m_mode == EdgeMode::FoldRepeat; }

        uintptr_t size1() const         { return m_size1; }
        uintptr_t size2() const         { return m_size2; }
        uintptr_t min() const           { return std::min(m_size1, m_size2); }
        uintptr_t max() const           { return std::max(m_size1, m_size2); }
        uintptr_t linear() const        { return m_size1 + m_size2 - 1; }
        uintptr_t fold_copy() const     { return max() + ((min() >> 1) << 1); }
        uintptr_t fft() const           { return uintptr_t(1) << m_fft_size_log2; }
        uintptr_t fft_log2() const      { return m_fft_size_log2; }
        
    private:

        uintptr_t calc_size() const
        {
            if (!foldMode())
                return linear();
            else
                return fold_copy() + (min() - 1);
        }
        
        EdgeMode m_mode;
        uintptr_t m_size1, m_size2, m_fft_size_log2;
    };
    
    // Folding copy
    
   static void fold(T* io, uintptr_t size, uintptr_t fold_size, bool repeat)
    {
        const T* first = io + fold_size;
        const T* last = io + fold_size + size;
        size_t offset = repeat ? 0 : 1;
        
        std::reverse_copy(first + offset, first + fold_size + offset, io);
        std::reverse_copy(last - (fold_size + offset), last - offset, io + size + fold_size);
    }
    
    static void copy_fold(T* output, in_ptr in, uintptr_t fold_size, bool repeat)
    {
        std::copy_n(in.m_ptr, in.m_size, output + fold_size);
        fold(output, in.m_size, fold_size, repeat);
    }
    
    static void copy_padded(T* output, in_ptr in, uintptr_t size, uintptr_t offset)
    {
        std::copy_n(in.m_ptr, in.m_size, output + offset);
        std::fill_n(output + offset + in.m_size, size - in.m_size, 0);
    }
    
    static void copy_fold_zero(Split& output, in_ptr in1, in_ptr in2, uintptr_t size, uintptr_t fold_size, bool repeat)
    {
        uintptr_t max_size = std::max(in1.m_size, in2.m_size);
        uintptr_t folded = max_size + (fold_size << 1);
        
        copy_padded(output.realp, in1, max_size, fold_size);
        fold(output.realp, max_size, fold_size, repeat);
        std::fill_n(output.realp + folded, size - folded, 0);
        copy_padded(output.imagp, in2, max_size, fold_size);
        fold(output.imagp, max_size, fold_size, repeat);
        std::fill_n(output.imagp + folded, size - folded, 0);
    }
    
    // Memory manipulation (complex)
    
    static void copy(Split& output, const Split& spectrum, uintptr_t o_offset, uintptr_t offset, uintptr_t size)
    {
        std::copy_n(spectrum.realp + offset, size, output.realp + o_offset);
        std::copy_n(spectrum.imagp + offset, size, output.imagp + o_offset);
    }
    
    static void wrap(Split& output, const Split& spectrum, uintptr_t o_offset, uintptr_t offset, uintptr_t size)
    {
        for (uintptr_t i = 0; i < size; i++)
        {
            output.realp[i + o_offset] += spectrum.realp[i + offset];
            output.imagp[i + o_offset] += spectrum.imagp[i + offset];
        }
    }
    
    static void zero(Split& output, uintptr_t start, uintptr_t end)
    {
        for (uintptr_t i = start; i < end; i++)
        {
            output.realp[i] = T(0);
            output.imagp[i] = T(0);
        }
    }

    // Memory manipulation (real)
    
    static void copy(T *output, const Split& spectrum, uintptr_t o_offset, uintptr_t offset, uintptr_t size)
    {
        zipped_pointer p(spectrum, offset);

        for (uintptr_t i = 0; i < size; i++)
            output[o_offset + i] = *p++;
    }
    
    static void wrap(T *output, const Split& spectrum, uintptr_t o_offset, uintptr_t last, uintptr_t size)
    {
        zipped_pointer p(spectrum, last - size);
        
        for (uintptr_t i = 0; i < size; i++)
            output[o_offset + i] += *p++;
    }
    
    static void zero(T *output, uintptr_t start, uintptr_t end)
    {
        for (uintptr_t i = start; i < end; i++)
            output[i] = T(0);
    }
    
    // Arranges for convolution and correlation
    
    template <class U>
    static void arrange_convolve(U output, Split spectrum, op_sizes& sizes)
    {
        uintptr_t min_m1 = (sizes.min() - 1);
        
        switch (sizes.mode())
        {
            case EdgeMode::Linear:
            {
                copy(output, spectrum, 0, 0, sizes.linear());
                break;
            }
                
            case EdgeMode::Wrap:
            {
                copy(output, spectrum, 0, 0, sizes.max());
                wrap(output, spectrum, 0, sizes.linear(), min_m1);
                break;
            }
        
            case EdgeMode::WrapCentre:
            {
                uintptr_t wrapped = min_m1 >> 1;
                copy(output, spectrum, 0, wrapped, sizes.max());
                wrap(output, spectrum, 0, sizes.linear(), min_m1 - wrapped);
                wrap(output, spectrum, sizes.max() - wrapped, wrapped, wrapped);
                break;
            }
                
            case EdgeMode::Fold:
            case EdgeMode::FoldRepeat:
            {
                copy(output, spectrum, 0, min_m1, sizes.max());
                break;
            }
        }
    }

    template <class U>
    static void arrange_correlate(U output, Split spectrum, op_sizes& sizes)
    {
        uintptr_t size2_m1 = sizes.size2() - 1;
    
        switch (sizes.mode())
        {
            case EdgeMode::Linear:
            {
                copy(output, spectrum, 0, 0, sizes.size1());
                copy(output, spectrum, sizes.size1(), sizes.fft() - size2_m1, size2_m1);
                break;
            }
                
            case EdgeMode::Wrap:
            {
                copy(output, spectrum, 0, 0, sizes.size1());
                zero(output, sizes.size1(), sizes.size2());
                wrap(output, spectrum, sizes.max() - size2_m1, sizes.fft(), size2_m1);
                break;
            }
                
            case EdgeMode::WrapCentre:
            {
                uintptr_t wrapped1 = (sizes.min() - 1) >> 1;
                uintptr_t wrapped2 = std::min(size2_m1, sizes.max() - wrapped1);
                uintptr_t wrapped3 = size2_m1 - wrapped2;
                
                uintptr_t offset = wrapped3 ? 0 : sizes.max() - (size2_m1 + wrapped1);
                
                zero(output, 0, sizes.max());
                copy(output, spectrum, 0, wrapped1, sizes.size1() - wrapped1);
                copy(output, spectrum, sizes.max() - wrapped1, 0, wrapped1);
                wrap(output, spectrum, offset, sizes.fft(), wrapped2);
                wrap(output, spectrum, sizes.max() - wrapped3, sizes.fft() - wrapped2, wrapped3);
                break;
            }
            
            case EdgeMode::Fold:
            case EdgeMode::FoldRepeat:
            {
                if (sizes.size1() >= sizes.size2())
                {
                    copy(output, spectrum, 0, 0, sizes.max());
                }
                else
                {
                    uintptr_t copy_size = sizes.max() - 1;

                    copy(output, spectrum, 0, 0, 1);
                    copy(output, spectrum, 1, sizes.fft() - copy_size, copy_size);
                }
                break;
            }
        }
    }
   
    // Binary Operations
    
    typedef void (*SpectralOp)(Split *, Split *, Split *, uintptr_t, T);
    typedef void (*ComplexArrange)(Split, Split, op_sizes&);
    typedef void (*RealArrange)(T *, Split, op_sizes&);

    uintptr_t calc_conv_corr_size(uintptr_t size1, uintptr_t size2, EdgeMode mode) const
    {
        if (!size1 || !size2)
            return 0;
        
        op_sizes sizes(size1, size2, mode);
        
        if ((sizes.fft() > max_fft_size()))
            return 0;
        
        return mode != EdgeMode::Linear ? sizes.max() : sizes.linear();
    }
    
    template<SpectralOp Op>
    void binary_op(Split& io, Split& temp, op_sizes& sizes, in_ptr r_in1, in_ptr i_in1, in_ptr r_in2, in_ptr i_in2)
    {
        bool fold1 = sizes.foldMode() && sizes.size1() >= sizes.size2();
        bool fold2 = sizes.foldMode() && !fold1;
        bool repeat = sizes.mode() == EdgeMode::FoldRepeat;
        uintptr_t fold_size = sizes.min() >> 1;
        
        copy_fold_zero(io, r_in1, i_in1, sizes.fft(), fold1 ? fold_size : 0, repeat);
        copy_fold_zero(temp, r_in2, i_in2, sizes.fft(), fold2 ? fold_size : 0, repeat);
        
        fft(io, sizes.fft_log2());
        fft(temp, sizes.fft_log2());
        
        Op(&io, &io, &temp, sizes.fft(), 1.0 / (T) sizes.fft());
        
        ifft(io, sizes.fft_log2());
    }
    
    template<SpectralOp Op, ComplexArrange arrange>
    void binary_op(T *r_out, T *i_out, in_ptr r_in1, in_ptr i_in1, in_ptr r_in2, in_ptr i_in2, EdgeMode mode)
    {
        auto get_first = [](in_ptr ptr)
        {
            return ptr.m_size ? ptr.m_ptr[0] : 0.0;
        };
        
        uintptr_t size1 = std::max(r_in1.m_size, i_in1.m_size);
        uintptr_t size2 = std::max(r_in2.m_size, i_in2.m_size);
        
        if (!calc_conv_corr_size(size1, size2, mode))
            return;
        
        // Special case for single sample inputs

        if (size1 == 1 && size2 == 1)
        {
            r_out[0] = get_first(r_in1) * get_first(r_in2) - get_first(i_in1) * get_first(i_in2);
            i_out[0] = get_first(r_in1) * get_first(i_in2) + get_first(i_in1) * get_first(r_in2);
            return;
        }
        
        // Assign temporary memory
        
        op_sizes sizes(size1, size2, mode);
        temporary_buffers<2> buffers(m_allocator, sizes.fft());
        Split output {r_out, i_out};
        
        // Process
        
        if (buffers)
        {
            binary_op<Op>(buffers.m_spectra[0], buffers.m_spectra[1], sizes, r_in1, i_in1, r_in2, i_in2);
            arrange(output, buffers.m_spectra[0], sizes);
        }
    }
    
    template<SpectralOp Op>
    void binary_op(Split& io, Split& temp, op_sizes& sizes, in_ptr in1, in_ptr in2)
    {
        if (!sizes.foldMode())
        {
            rfft(io, in1.m_ptr, in1.m_size, sizes.fft_log2());
            rfft(temp, in2.m_ptr, in2.m_size, sizes.fft_log2());
        }
        else
        {
            uintptr_t fold_size = sizes.min() >> 1;
            bool repeat = sizes.mode() == EdgeMode::FoldRepeat;

            if (sizes.size1() >= sizes.size2())
            {
                copy_fold(temp.realp, in1, fold_size, repeat);
                rfft(io, temp.realp, sizes.fold_copy(), sizes.fft_log2());
                rfft(temp, in2.m_ptr, in2.m_size, sizes.fft_log2());
            }
            else
            {
                copy_fold(io.realp, in2, fold_size, repeat);
                rfft(temp, io.realp, sizes.fold_copy(), sizes.fft_log2());
                rfft(io, in1.m_ptr, in1.m_size, sizes.fft_log2());
            }
        }

        Op(&io, &io, &temp, sizes.fft(), 0.25 / (T) sizes.fft());
    
        rifft(io, sizes.fft_log2());
    }
    
    template<SpectralOp Op, RealArrange arrange>
    void binary_op(T *output, in_ptr in1, in_ptr in2, EdgeMode mode)
    {
        if (!calc_conv_corr_size(in1.m_size, in2.m_size, mode))
            return;
        
        // Special case for single sample inputs

        if (in1.m_size == 1 && in2.m_size == 1)
        {
            output[0] = in1.m_ptr[0] * in2.m_ptr[0];
            return;
        }
        
        // Assign temporary memory
        
        op_sizes sizes(in1.m_size, in2.m_size, mode);
        temporary_buffers<2> buffers(m_allocator, sizes.fft() >> 1);
        
        // Process
        
        if (buffers)
        {
            binary_op<Op>(buffers.m_spectra[0], buffers.m_spectra[1], sizes, in1, in2);
            arrange(output, buffers.m_spectra[0], sizes);
        }
    }
    
    // Data
    
    Allocator m_allocator;
    Setup m_fft_setup;
    uintptr_t m_max_fft_size_log2;
};

#endif
