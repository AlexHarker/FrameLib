
#ifndef SPECTRALFUNCTIONS_HPP
#define SPECTRALFUNCTIONS_HPP

#include "HISSTools_FFT/HISSTools_FFT.h"
#include "SIMDSupport.hpp"

#include <algorithm>
#include <cmath>
#include <complex>

namespace impl
{
    template <typename T>
    struct Infer {};
    
    template <>
    struct Infer<FFT_SPLIT_COMPLEX_D>
    {
        using Setup = FFT_SETUP_D;
        using Type = double;
    };
    
    template <>
    struct Infer<FFT_SPLIT_COMPLEX_F>
    {
        using Setup = FFT_SETUP_F;
        using Type = float;
    };
    
    template<int N, typename Split, typename Op>
    void simd_operation(Split *out, Split *in1, Split *in2, uintptr_t fft_size, double scale, Op op)
    {
        using VecType = SIMDType<typename Infer<Split>::Type, N>;
        
        const VecType *r_in1 = reinterpret_cast<const VecType *>(in1->realp);
        const VecType *i_in1 = reinterpret_cast<const VecType *>(in1->imagp);
        const VecType *r_in2 = reinterpret_cast<const VecType *>(in2->realp);
        const VecType *i_in2 = reinterpret_cast<const VecType *>(in2->imagp);
        VecType *r_out = reinterpret_cast<VecType *>(out->realp);
        VecType *i_out = reinterpret_cast<VecType *>(out->imagp);
        
        VecType v_scale(scale);
        
        for (uintptr_t i = 0; i < (fft_size / N); i++)
            op(r_out[i], i_out[i], r_in1[i], i_in1[i], r_in2[i], i_in2[i], v_scale, i);
    }
    
    template<typename Split, typename Op>
    void complex_operation(Split *out, Split *in1, Split *in2, uintptr_t fft_size, typename Infer<Split>::Type scale, Op op)
    {
        const int N = SIMDLimits<typename Infer<Split>::Type>::max_size;
        constexpr int M = N / 2 ? N / 2 : 1;
        
        if (fft_size == 1 || fft_size < M)
            simd_operation<1>(out, in1, in2, fft_size, scale, op);
        else if (fft_size < N)
            simd_operation<M>(out, in1, in2, fft_size, scale, op);
        else
            simd_operation<N>(out, in1, in2, fft_size, scale, op);
    }
    
    template<typename Split, typename Op>
    void real_operation(Split *out, Split *in1, Split *in2, uintptr_t fft_size, typename Infer<Split>::Type scale, Op op)
    {
        using T = typename Infer<Split>::Type;

        T temp1(0);
        T temp2(0);
        T dc_value;
        T nq_value;
        
        // DC and Nyquist
        
        op(dc_value, temp1, in1->realp[0], temp2, in2->realp[0], temp2, scale, 0);
        op(nq_value, temp1, in1->imagp[0], temp2, in2->imagp[0], temp2, scale, fft_size >> 1);
        
        complex_operation(out, in1, in2, fft_size >> 1, scale, op);
        
        // Set DC and Nyquist bins
        
        out->realp[0] = dc_value;
        out->imagp[0] = nq_value;
    }
    
    template <typename Split, typename Op>
    void real_operation(Split *out, const Split *in, uintptr_t fft_size, Op op)
    {
        using T = typename Infer<Split>::Type;

        const T *r_in = in->realp;
        const T *i_in = in->imagp;
        T *r_out = out->realp;
        T *i_out = out->imagp;
        
        T temp1(0);
        T temp2(0);
        
        // DC and Nyquist
        
        op(r_out[0], temp1, r_in[0], temp2, 0);
        op(i_out[0], temp1, i_in[0], temp2, fft_size >> 1);
        
        // Other bins
        
        for (uintptr_t i = 1; i < (fft_size >> 1); i++)
            op(r_out[i], i_out[i], r_in[i], i_in[i], i);
    }
    
    template <typename Split, typename Op>
    void real_operation(Split *out, uintptr_t fft_size, Op op)
    {
        using T = typename Infer<Split>::Type;
        
        T *r_out = out->realp;
        T *i_out = out->imagp;
        
        T temp(0);
        
        // DC and Nyquist
        
        op(r_out[0], temp, 0);
        op(i_out[0], temp, fft_size >> 1);
        
        // Other bins
        
        for (uintptr_t i = 1; i < (fft_size >> 1); i++)
            op(r_out[i], i_out[i], i);
    }
    
    template <class T>
    void store(T& r_out, T& i_out, T&& r_in, T&& i_in)
    {
        r_out = r_in;
        i_out = i_in;
    }
    
    // Functors
    
    struct copy
    {
        template <typename T>
        void operator()(T& r_out, T& i_out, const T& r_in, const T& i_in, uintptr_t i)
        {
            store(r_out, i_out, T(r_in), T(i_in));
        }
    };
    
    struct amplitude
    {
        template <typename T>
        void operator()(T& r_out, T& i_out, const T& r_in, const T& i_in, uintptr_t i)
        {
            store(r_out, i_out, std::sqrt(r_in * r_in + i_in * i_in), T(0));
        }
    };
    
    struct amplitude_linear
    {
        template <typename T>
        void operator()(T& r_out, T& i_out, const T& r_in, const T& i_in, uintptr_t i)
        {
            store(r_out, i_out, std::sqrt(r_in * r_in + i_in * i_in) * (i & 0x1 ? T(-1) : T(1)), T(0));
        }
    };
    
    struct conjugate
    {
        template <typename T>
        void operator()(T& r_out, T& i_out, const T& r_in, const T& i_in, uintptr_t i)
        {
            store(r_out, i_out, T(r_in), T(-i_in));
        }
    };
    
    struct log_power
    {
        template <typename T>
        void operator()(T& r_out, T& i_out, const T& r_in, const T& i_in, uintptr_t i)
        {
            static T min_power = std::pow(10.0, -300.0 / 10.0);
            store(r_out, i_out, T(0.5) * std::log(std::max(r_in * r_in + i_in * i_in, min_power)), T(0));
        }
    };
    
    struct complex_exponential
    {
        template <typename T>
        void operator()(T& r_out, T& i_out, const T& r_in, const T& i_in, uintptr_t i)
        {
            const std::complex<T> c = std::exp(std::complex<T>(r_in, i_in));
            store(r_out, i_out, std::real(c), std::imag(c));
        }
    };
    
    struct complex_exponential_conjugate
    {
        template <typename T>
        void operator()(T& r_out, T& i_out, const T& r_in, const T& i_in, uintptr_t i)
        {
            const std::complex<T> c = std::exp(std::complex<T>(r_in, i_in));
            store(r_out, i_out, std::real(c), -std::imag(c));
        }
    };
    
    struct phase_interpolate
    {
        phase_interpolate(double phase, uintptr_t fft_size, bool zero_center)
        {
            // N.B. - induce a delay of -1 sample for anything over linear to avoid wraparound
            
            const double delay_factor = (phase <= 0.5) ? 0.0 : 1.0 / static_cast<double>(fft_size);
            
            phase = std::max(0.0, std::min(1.0, phase));
            min_factor = 1.0 - (2.0 * phase);
            lin_factor = zero_center ? 0.0 : (-2.0 * M_PI * (phase - delay_factor));
        }
        
        template <typename T>
        void operator()(T& r_out, T& i_out, const T& r_in, const T& i_in, uintptr_t i)
        {
            const double amp = std::exp(r_in);
            const double phase = lin_factor * i + min_factor * i_in;
            store(r_out, i_out, T(amp * std::cos(phase)), T(amp * std::sin(phase)));
        }
        
        double min_factor;
        double lin_factor;
    };
    
    struct spike
    {
        spike(double position, uintptr_t fft_size)
        {
            spike_constant = ((long double) (2.0 * M_PI)) *  -position / static_cast<double>(fft_size);
        }
        
        template <typename T>
        void operator()(T& r_out, T& i_out, uintptr_t i)
        {
            const long double phase = spike_constant * i;
            
            store(r_out, i_out, static_cast<T>(std::cos(phase)), static_cast<T>(std::sin(phase)));
        }
        
        long double spike_constant;
    };
    
    struct delay_calc : private spike
    {
        delay_calc(double delay, uintptr_t fft_size) : spike(delay, fft_size) {}
        
        template <typename T>
        void operator()(T& r_out, T& i_out, const T& r_in, const T& i_in, uintptr_t i)
        {
            using complex = std::complex<T>;
            
            const long double phase = spike::spike_constant * i;
            const complex c = complex(r_in, i_in) * complex(std::cos(phase), std::sin(phase));
            
            store(r_out, i_out, std::real(c), std::imag(c));
        }
    };
    
    struct correlate
    {
        template<class T>
        void operator()(T& r_out, T& i_out, const T& a, const T& b, const T& c, const T& d, const T& scale, uintptr_t i)
        {
            store(r_out, i_out, scale * (a * c + b * d), scale * (b * c - a * d));
        }
    };
    
    struct convolve
    {
        template<class T>
        void operator()(T& r_out, T& i_out, const T& a, const T& b, const T& c, const T& d, const T& scale, uintptr_t i)
        {
            store(r_out, i_out, scale * (a * c - b * d), scale * (b * c + a * d));
        }
    };
    
    template <typename Split>
    void minimum_phase_components(typename Infer<Split>::Setup setup, Split *out, Split *in, uintptr_t fft_size)
    {
        using T = typename Infer<Split>::Type;
        
        T *r_out = out->realp;
        T *i_out = out->imagp;
        
        // From the full FFT size calculate the size log2
        
        uintptr_t fft_size_log2 = 0;
        
        for (uintptr_t i = fft_size; i; i >>= 1)
            fft_size_log2++;
        
        fft_size_log2--;
        
        // Take Log of Power Spectrum
        
        real_operation(out, in, fft_size, log_power());
        
        // Do Real iFFT
        
        hisstools_rifft(setup, out, fft_size_log2);
        
        // Double Causal Values / Zero Non-Casual Values / Scale All Remaining
        
        // N.B. - doubling is implicit because the real FFT will double the result
        //      - (0.5 multiples needed where no doubling should take place)
        
        double scale = 1.0 / fft_size;
        
        r_out[0] *= 0.5 * scale;
        i_out[0] *= scale;
        
        for (uintptr_t i = 1; i < (fft_size >> 2); i++)
        {
            r_out[i] *= scale;
            i_out[i] *= scale;
        }
        
        r_out[fft_size >> 2] *= 0.5 * scale;
        i_out[fft_size >> 2] = 0.0;
        
        for (uintptr_t i = (fft_size >> 2) + 1; i < (fft_size >> 1); i++)
        {
            r_out[i] = 0.0;
            i_out[i] = 0.0;
        }
        
        // Forward Real FFT (here there is a scaling issue to consider that is compensated above)
        
        hisstools_rfft(setup, out, fft_size_log2);
    }
}

// Types

template <typename T>
struct FFTTypes
{
    using Split = void;
    using Setup = void;
};

template<>
struct FFTTypes<float>
{
    using Split = FFT_SPLIT_COMPLEX_F;
    using Setup = FFT_SETUP_F;
};

template<>
struct FFTTypes<double>
{
    using Split = FFT_SPLIT_COMPLEX_D;
    using Setup = FFT_SETUP_D;
};

// Function calls

template <typename Split>
void ir_copy(Split *out, const Split *in, uintptr_t fft_size)
{
    impl::real_operation(out, in, fft_size, impl::copy());
}

template <typename Split>
void ir_spike(Split *out, uintptr_t fft_size, double spike_position)
{
    impl::real_operation(out, fft_size, impl::spike(spike_position, fft_size));
}

template <typename Split>
void ir_delay(Split *out, const Split *in, uintptr_t fft_size, double delay)
{
    if (delay != 0.0)
        impl::real_operation(out, in, fft_size, impl::delay_calc(delay, fft_size));
    else if (in != out)
        ir_copy(out, in, fft_size);
}

template <typename Split>
void ir_time_reverse(Split *out, const Split *in, uintptr_t fft_size)
{
    impl::real_operation(out, in, fft_size, impl::conjugate());
}

template <typename Setup, typename Split>
void ir_phase(Setup setup, Split *out, Split *in, uintptr_t fft_size, double phase, bool zero_center = false)
{
    if (phase == 0.5)
    {
        if (zero_center)
            impl::real_operation(out, in, fft_size, impl::amplitude());
        else
            impl::real_operation(out, in, fft_size, impl::amplitude_linear());
    }
    else
    {
        impl::minimum_phase_components(setup, out, in, fft_size);
        
        if (phase == 1.0 && zero_center)
            impl::real_operation(out, out, fft_size, impl::complex_exponential_conjugate());
        else if (phase == 0.0)
            impl::real_operation(out, out, fft_size, impl::complex_exponential());
        else
            impl::real_operation(out, out, fft_size, impl::phase_interpolate(phase, fft_size, zero_center));
    }
}

template <typename Split, typename T>
void ir_convolve_complex(Split *out, Split *in1, Split *in2, uintptr_t fft_size, T scale)
{
    impl::complex_operation(out, in1, in2, fft_size, scale, impl::convolve());
}

template <typename Split, typename T>
void ir_convolve_real(Split *out, Split *in1, Split *in2, uintptr_t fft_size, T scale)
{
    impl::real_operation(out, in1, in2, fft_size, scale, impl::convolve());
}

template <typename Split, typename T>
void ir_correlate_complex(Split *out, Split *in1, Split *in2, uintptr_t fft_size, T scale)
{
    impl::complex_operation(out, in1, in2, fft_size, scale, impl::correlate());
}

template <typename Split, typename T>
void ir_correlate_real(Split *out, Split *in1, Split *in2, uintptr_t fft_size, T scale)
{
    impl::real_operation(out, in1, in2, fft_size, scale, impl::correlate());
}

#endif
