

#ifndef __HISSTOOLS_FFT__
#define __HISSTOOLS_FFT__

// Comment out the following line if you don't wish to use the apple fft when available

#define USE_APPLE_FFT_IF_AVAILABLE

#define _USE_MATH_DEFINES

#include <math.h>
#include "FFT_Header.h"

// Platform check

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#ifdef USE_APPLE_FFT_IF_AVAILABLE
#define USE_APPLE_FFT
#endif
#endif

// Apple FFT

#ifdef USE_APPLE_FFT

#define FFT_SETUP_F FFTSetup
#define FFT_SETUP_D FFTSetupD

#define FFT_SPLIT_COMPLEX_D DSPDoubleSplitComplex
#define FFT_SPLIT_COMPLEX_F DSPSplitComplex

#else

#define FFT_SETUP_F FFTSetupFloat *
#define FFT_SETUP_D FFTSetupDouble *

#define FFT_SPLIT_COMPLEX_F SplitFloat
#define FFT_SPLIT_COMPLEX_D SplitDouble

#endif

#ifdef __cplusplus
extern "C"  {
#endif

// FFT Routines

// Complex 

void hisstools_fft_d (FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, HstFFT_UInt log2n);
void hisstools_fft_f (FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, HstFFT_UInt log2n);

// Real

void hisstools_rfft_d (FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, HstFFT_UInt log2n);
void hisstools_rfft_f (FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, HstFFT_UInt log2n);

// Complex Inverse

void hisstools_ifft_d (FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, HstFFT_UInt log2n);
void hisstools_ifft_f (FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, HstFFT_UInt log2n);

// Real Inverse

void hisstools_rifft_d (FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, HstFFT_UInt log2n);
void hisstools_rifft_f (FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, HstFFT_UInt log2n);

// Unzip incorporating zero padding

void hisstools_unzip_zero_d (double *input, FFT_SPLIT_COMPLEX_D *output, HstFFT_UInt in_length, HstFFT_UInt log2n);
void hisstools_unzip_zero_f (float *input, FFT_SPLIT_COMPLEX_F *output, HstFFT_UInt in_length, HstFFT_UInt log2n);

// Special float to double precision unzip routine

void hisstools_unzip_zero_fd (float *input, FFT_SPLIT_COMPLEX_D *output, HstFFT_UInt in_length, HstFFT_UInt log2n);

// Zip and Unzip

void hisstools_unzip_d (double *input, FFT_SPLIT_COMPLEX_D *output, HstFFT_UInt log2n);
void hisstools_unzip_f (float *input, FFT_SPLIT_COMPLEX_F *output, HstFFT_UInt log2n);
void hisstools_zip_d (FFT_SPLIT_COMPLEX_D *input, double *output, HstFFT_UInt log2n);
void hisstools_zip_f (FFT_SPLIT_COMPLEX_F *input, float *output, HstFFT_UInt log2n);

// Setup Create / Destory

FFT_SETUP_D hisstools_create_setup_d(HstFFT_UInt max_fft_log_2);
FFT_SETUP_F hisstools_create_setup_f(HstFFT_UInt max_fft_log_2);
void hisstools_destroy_setup_d (FFT_SETUP_D setup);
void hisstools_destroy_setup_f (FFT_SETUP_F setup);

#ifdef __cplusplus
}
#endif

#endif

