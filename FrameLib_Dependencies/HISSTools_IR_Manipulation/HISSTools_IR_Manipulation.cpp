
#include "HISSTools_IR_Manipulation.h"

#include <algorithm>
#include <cmath>
#include <complex>

template <typename T>
struct BaseType {};

template <>
struct BaseType <FFT_SPLIT_COMPLEX_D> { using type = double; };

template <>
struct BaseType <FFT_SPLIT_COMPLEX_F> { using type = float; };


template <typename T, typename Op>
void ir_real_operation(T *out, uintptr_t fft_size, Op op)
{
    typename BaseType<T>::type *r_out = out->realp;
    typename BaseType<T>::type *i_out = out->imagp;
    
    typename BaseType<T>::type temp(0);
    
    // DC and Nyquist
    
    op(r_out[0], temp, 0);
    op(i_out[0], temp, fft_size >> 1);
    
    // Other bins
    
    for (uintptr_t i = 1; i < (fft_size >> 1); i++)
        op(r_out[i], i_out[i], i);
}

template <typename T, typename Op>
void ir_real_operation(T *out, const T *in, uintptr_t fft_size, Op op)
{
    const typename BaseType<T>::type *r_in = in->realp;
    const typename BaseType<T>::type *i_in = in->imagp;
    typename BaseType<T>::type *r_out = out->realp;
    typename BaseType<T>::type *i_out = out->imagp;
    
    typename BaseType<T>::type temp1(0);
    typename BaseType<T>::type temp2(0);

    // DC and Nyquist
    
    op(r_out[0], temp1, r_in[0], temp1, 0);
    op(i_out[0], temp2, i_in[0], temp2, fft_size >> 1);
    
    // Other bins
    
    for (uintptr_t i = 1; i < (fft_size >> 1); i++)
        op(r_out[i], i_out[i], r_in[i], i_in[i], i);
}

template <class T>
void store(T& r_out, T& i_out, T r_in, T i_in)
{
    r_out = r_in;
    i_out = i_in;
}

// Functors

template <typename T>
struct copy
{
    void operator()(T& r_out, T& i_out, T r_in, T i_in, uintptr_t i)
    {
        store(r_out, i_out, r_in, i_in);
    }
};

template <typename T>
struct amplitude
{
    void operator()(T& r_out, T& i_out, T r_in, T i_in, uintptr_t i)
    {
        store(r_out, i_out, std::sqrt(r_in * r_in + i_in * i_in), T(0));
    }
};

template <typename T>
struct amplitude_flip
{
    void operator()(T& r_out, T& i_out, T r_in, T i_in, uintptr_t i)
    {
        store(r_out, i_out, std::sqrt(r_in * r_in + i_in * i_in) * (i & 0x1 ? T(-1) : T(1)), T(0));
    }
};

template <typename T>
struct conjugate
{
    void operator()(T& r_out, T& i_out, T r_in, T i_in, uintptr_t i)
    {
        store(r_out, i_out, r_in, -i_in);
    }
};

template <typename T>
struct log_power
{
    void operator()(T& r_out, T& i_out, T r_in, T i_in, uintptr_t i)
    {
        static T min_power = std::pow(10.0, -300.0 / 10.0);
        store(r_out, i_out, T(0.5) * log(std::max(r_in * r_in + i_in * i_in, min_power)), T(0));
    }
};

template <typename T>
struct complex_exponential
{
    void operator()(T& r_out, T& i_out, T r_in, T i_in, uintptr_t i)
    {
        const std::complex<T> c = std::exp(std::complex<T>(r_in, i_in));
        store(r_out, i_out, std::real(c), std::imag(c));
    }
};

template <typename T>
struct complex_exponential_conjugate
{
    void operator()(T& r_out, T& i_out, T r_in, T i_in, uintptr_t i)
    {
        const std::complex<T> c = std::exp(std::complex<T>(r_in, i_in));
        store(r_out, i_out, std::real(c), -std::imag(c));
    }
};

template <typename T>
struct phase_interpolate
{
    phase_interpolate(double phase, double fft_size, bool zero_center)
    {
        // N.B. - induce a delay of -1 sample for anything over linear to avoid wraparound
        
        const double delay_factor = (phase <= 0.5) ? 0.0 : 1.0 / fft_size;
        
        phase = std::max(0.0, std::min(1.0, phase));
        min_factor = 1.0 - (2.0 * phase);
        lin_factor = zero_center ? 0.0 : (-2.0 * M_PI * (phase - delay_factor));
    }
    
    void operator()(T& r_out, T& i_out, T r_in, T i_in, uintptr_t i)
    {
        const double amp = std::exp(r_in);
        const double phase = lin_factor * i + min_factor * i_in;
        store(r_out, i_out, T(amp * std::cos(phase)), T(amp * std::sin(phase)));
    }
    
    double min_factor;
    double lin_factor;
};

template <typename T>
struct spike
{
    spike(double position, double fft_size)
    {
        spike_constant = ((long double) (2.0 * M_PI)) *  -position / fft_size;
    }
    
    void operator()(T& r_out, T& i_out, uintptr_t i)
    {
        const long double phase = spike_constant * i;
        
        store(r_out, i_out, static_cast<T>(std::cos(phase)), static_cast<T>(std::sin(phase)));
    }
    
    long double spike_constant;
};

template <typename T>
struct delay_calc : private spike<T>
{
    delay_calc(double delay, double fft_size) : spike<T>(delay, fft_size) {}
    
    void operator()(T& r_out, T& i_out, T r_in, T i_in, uintptr_t i)
    {
        using complex = std::complex<T>;
        
        const long double phase = spike<T>::spike_constant * i;
        const complex c = complex(r_in, i_in) * complex(std::cos(phase), std::sin(phase));
        
        store(r_out, i_out, std::real(c), std::imag(c));
    }
};

template <typename T>
void ir_delay_impl(T *out, const T *in, uintptr_t fft_size, double delay)
{
    if (delay != 0.0)
        ir_real_operation(out, in, fft_size, delay_calc<typename BaseType<T>::type>(delay, fft_size));
    else if (in != out)
        ir_copy(out, in, fft_size);
}

template <class SETUP, class SPLIT>
void minimum_phase_components(SETUP setup, SPLIT *out, SPLIT *in, uintptr_t fft_size)
{
    using T = typename BaseType<SPLIT>::type;
    
    // FIX - what is this value?
    
    uintptr_t fft_size_log2 = 0;
    
    for (uintptr_t i = fft_size; i; i >>= 1)
        fft_size_log2++;
    
    fft_size_log2--;
    
    // Take Log of Power Spectrum
    
    ir_real_operation(out, out, fft_size, log_power<T>());
    
    // Do Real iFFT
    
    hisstools_rifft(setup, out, fft_size_log2);
    
    // Double Causal Values / Zero Non-Casual Values / Scale All Remaining
    
    // N.B. - doubling is implicit because the real FFT will double the result
    //      - (0.5 multiples needed where no doubling should take place)
    
    double scale = 1.0 / fft_size;
    
    out->realp[0] *= 0.5 * scale;
    out->imagp[0] *= scale;
    
    for (uintptr_t i = 1; i < (fft_size >> 2); i++)
    {
        out->realp[i] *= scale;
        out->imagp[i] *= scale;
    }
    
    out->realp[fft_size >> 2] *= 0.5 * scale;
    out->imagp[fft_size >> 2] = 0.0;
    
    for (unsigned long i = (fft_size >> 2) + 1; i < (fft_size >> 1); i++)
    {
        out->realp[i] = 0.0;
        out->imagp[i] = 0.0;
    }
    
    // Forward Real FFT (here there is a scaling issue to consider)
    
    hisstools_rfft(setup, out, fft_size_log2);
}

template <class SETUP, class SPLIT>
void ir_phase_impl(SETUP setup, SPLIT *out, SPLIT *in, uintptr_t fft_size, double phase, bool zero_center)
{
    using T = typename BaseType<SPLIT>::type;
    
    if (phase == 0.5)
    {
        if (zero_center)
            ir_real_operation(out, in, fft_size, amplitude<T>());
        else
            ir_real_operation(out, in, fft_size, amplitude_flip<T>());
    }
    else
    {
        minimum_phase_components(setup, out, in, fft_size);
        
        if (phase == 1.0 && zero_center)
            ir_real_operation(out, out, fft_size, complex_exponential_conjugate<T>());
        else if (phase == 0.0)
            ir_real_operation(out, out, fft_size, complex_exponential<T>());
        else
            ir_real_operation(out, out, fft_size, phase_interpolate<T>(phase, fft_size, zero_center));
    }
}

// Concrete function calls

void ir_copy(FFT_SPLIT_COMPLEX_F *out, const FFT_SPLIT_COMPLEX_F *in, uintptr_t fft_size)
{
    ir_real_operation(out, in, fft_size, copy<float>());
}

void ir_copy(FFT_SPLIT_COMPLEX_D *out, const FFT_SPLIT_COMPLEX_D *in, uintptr_t fft_size)
{
    ir_real_operation(out, in, fft_size, copy<double>());
}

void ir_spike(FFT_SPLIT_COMPLEX_F *out, uintptr_t fft_size, double spike_position)
{
    ir_real_operation(out, fft_size, spike<float>(spike_position, fft_size));
}

void ir_spike(FFT_SPLIT_COMPLEX_D *out, uintptr_t fft_size, double spike_position)
{
    ir_real_operation(out, fft_size, spike<double>(spike_position, fft_size));
}

void ir_delay(FFT_SPLIT_COMPLEX_F *out, const FFT_SPLIT_COMPLEX_F *in, uintptr_t fft_size, double delay)
{
    ir_delay_impl(out, in, fft_size, delay);
}

void ir_delay(FFT_SPLIT_COMPLEX_D *out, const FFT_SPLIT_COMPLEX_D *in, uintptr_t fft_size, double delay)
{
    ir_delay_impl(out, in, fft_size, delay);
}

void ir_time_reverse(FFT_SPLIT_COMPLEX_F *out, const FFT_SPLIT_COMPLEX_F *in, uintptr_t fft_size)
{
    ir_real_operation(out, in, fft_size, conjugate<float>());
}

void ir_time_reverse(FFT_SPLIT_COMPLEX_D *out, const FFT_SPLIT_COMPLEX_D *in, uintptr_t fft_size)
{
    ir_real_operation(out, in, fft_size, conjugate<double>());
}

void ir_phase(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *out, FFT_SPLIT_COMPLEX_F *in, uintptr_t fft_size, double phase, bool zero_center)
{
    ir_phase_impl(setup, out, in, fft_size, phase, zero_center);
}

void ir_phase(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *out, FFT_SPLIT_COMPLEX_D *in, uintptr_t fft_size, double phase, bool zero_center)
{
    ir_phase_impl(setup, out, in, fft_size, phase, zero_center);
}
