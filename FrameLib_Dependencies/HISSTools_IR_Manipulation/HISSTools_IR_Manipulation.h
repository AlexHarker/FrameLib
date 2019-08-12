
#ifndef __HISSTOOLS_IR_MANIPULATION__
#define __HISSTOOLS_IR_MANIPULATION__

#include "../HISSTools_FFT/HISSTools_FFT.h"

template <typename T> struct FFTTypes;

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


void ir_copy(FFT_SPLIT_COMPLEX_F *out, const FFT_SPLIT_COMPLEX_F *in, uintptr_t fft_size);
void ir_copy(FFT_SPLIT_COMPLEX_D *out, const FFT_SPLIT_COMPLEX_D *in, uintptr_t fft_size);

void ir_spike(FFT_SPLIT_COMPLEX_F *out, uintptr_t fft_size, double spike_position);
void ir_spike(FFT_SPLIT_COMPLEX_D *out, uintptr_t fft_size, double spike_position);

void ir_delay(FFT_SPLIT_COMPLEX_F *out, const FFT_SPLIT_COMPLEX_F *in, uintptr_t fft_size, double delay);
void ir_delay(FFT_SPLIT_COMPLEX_D *out, const FFT_SPLIT_COMPLEX_D *in, uintptr_t fft_size, double delay);

void ir_time_reverse(FFT_SPLIT_COMPLEX_F *out, const FFT_SPLIT_COMPLEX_F *in, uintptr_t fft_size);
void ir_time_reverse(FFT_SPLIT_COMPLEX_D *out, const FFT_SPLIT_COMPLEX_D *in, uintptr_t fft_size);

void ir_phase(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *out, FFT_SPLIT_COMPLEX_F *in, uintptr_t fft_size, double phase, bool zero_center = false);
void ir_phase(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *out, FFT_SPLIT_COMPLEX_D *in, uintptr_t fft_size, double phase, bool zero_center = false);

void ir_convolve_complex(FFT_SPLIT_COMPLEX_D *out, FFT_SPLIT_COMPLEX_D *in1, FFT_SPLIT_COMPLEX_D *in2, uintptr_t fft_size, double scale);
void ir_convolve_complex(FFT_SPLIT_COMPLEX_F *out, FFT_SPLIT_COMPLEX_F *in1, FFT_SPLIT_COMPLEX_F *in2, uintptr_t fft_size, float scale);
void ir_convolve_real(FFT_SPLIT_COMPLEX_D *out, FFT_SPLIT_COMPLEX_D *in1, FFT_SPLIT_COMPLEX_D *in2, uintptr_t fft_size, double scale);
void ir_convolve_real(FFT_SPLIT_COMPLEX_F *out, FFT_SPLIT_COMPLEX_F *in1, FFT_SPLIT_COMPLEX_F *in2, uintptr_t fft_size, float scale);

void ir_correlate_complex(FFT_SPLIT_COMPLEX_D *out, FFT_SPLIT_COMPLEX_D *in1, FFT_SPLIT_COMPLEX_D *in2, uintptr_t fft_size, double scale);
void ir_correlate_complex(FFT_SPLIT_COMPLEX_F *out, FFT_SPLIT_COMPLEX_F *in1, FFT_SPLIT_COMPLEX_F *in2, uintptr_t fft_size, float scale);
void ir_correlate_real(FFT_SPLIT_COMPLEX_D *out, FFT_SPLIT_COMPLEX_D *in1, FFT_SPLIT_COMPLEX_D *in2, uintptr_t fft_size, double scale);
void ir_correlate_real(FFT_SPLIT_COMPLEX_F *out, FFT_SPLIT_COMPLEX_F *in1, FFT_SPLIT_COMPLEX_F *in2, uintptr_t fft_size, float scale);

#endif
