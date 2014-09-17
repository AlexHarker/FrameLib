

#include "HISSTools_FFT.h"
#include "FFT_Main.h"

// FFT Routines

// Complex 

void hisstools_fft_d (FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT
	vDSP_fft_zipD(setup, input, (vDSP_Stride) 1, log2n, FFT_FORWARD);
#else
	do_fft(input, setup, log2n);
#endif
}

void hisstools_fft_f (FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT	
	vDSP_fft_zip(setup, input, (vDSP_Stride) 1, log2n, FFT_FORWARD);
#else
	do_fft_float(input, setup, log2n);
#endif
}

// Real

void hisstools_rfft_d (FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT	
	vDSP_fft_zripD(setup, input, (vDSP_Stride) 1, log2n, FFT_FORWARD);
#else
	do_real_fft(input, setup, log2n);
#endif
}	

void hisstools_rfft_f (FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT	
	vDSP_fft_zrip(setup, input, (vDSP_Stride) 1, log2n, FFT_FORWARD);
#else
	do_real_fft_float(input, setup, log2n);
#endif
}

// Complex Inverse

void hisstools_ifft_d (FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT
	vDSP_fft_zipD(setup, input, (vDSP_Stride) 1, log2n, FFT_INVERSE);
#else
	do_ifft(input, setup, log2n);
#endif
}

void hisstools_ifft_f (FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT	
	vDSP_fft_zip(setup, input, (vDSP_Stride) 1, log2n, FFT_INVERSE);
#else
	do_ifft_float(input, setup, log2n);
#endif
}

// Real Inverse

void hisstools_rifft_d (FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT	
	vDSP_fft_zripD(setup, input, (vDSP_Stride) 1, log2n, FFT_INVERSE);
#else
	do_real_ifft(input, setup, log2n);
#endif
}	

void hisstools_rifft_f (FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT	
	vDSP_fft_zrip(setup, input, (vDSP_Stride) 1, log2n, FFT_INVERSE);
#else
	do_real_ifft_float(input, setup, log2n);
#endif
}


// Unzip incorporating zero padding

void hisstools_unzip_zero_d (double *input, FFT_SPLIT_COMPLEX_D *output, HstFFT_UInt in_length, HstFFT_UInt log2n)
{
	HstFFT_UInt i;
	double temp = 0;
	
	double *realp = output->realp;
	double *imagp = output->imagp;
	
	// Check input length and get last value if in_length is odd
	
	if (((HstFFT_UInt) 1 << log2n) < in_length)
		in_length = (HstFFT_UInt) 1 << log2n;
	if (in_length & 1)
		temp = input[in_length - 1];
	
	// Unzip an even number of samples
		
#ifdef USE_APPLE_FFT	
	vDSP_ctozD((DOUBLE_COMPLEX *) input, (vDSP_Stride) 2, output, (vDSP_Stride) 1, in_length >> 1);
#else
	unzip_complex (input, output, in_length >> 1);
#endif
	
	// If necessary replace the odd sample, and zero pad the input
		
	if (((HstFFT_UInt) 1 << log2n) > in_length)
	{
		realp[in_length >> 1] = temp;
		imagp[in_length >> 1] = 0;
	
		for (i = (in_length >> (HstFFT_UInt) 1) + 1; i < ((HstFFT_UInt) 1 << (log2n - (HstFFT_UInt) 1)); i++)
		{
			realp[i] = 0.;
			imagp[i] = 0.;
		}
	}
}

void hisstools_unzip_zero_f (float *input, FFT_SPLIT_COMPLEX_F *output, HstFFT_UInt in_length, HstFFT_UInt log2n)
{
	HstFFT_UInt i;
	float temp = 0;
	
	float *realp = output->realp;
	float *imagp = output->imagp;
	
	// Check input length and get last value if in_length is odd
	
	if (((HstFFT_UInt) 1 << log2n) < in_length)
		in_length = (HstFFT_UInt) 1 << log2n;
	if (in_length & 1)
		temp = input[in_length - 1];
	
	// Unzip an even number of samples
		
#ifdef USE_APPLE_FFT	
	vDSP_ctoz((COMPLEX *) input, (vDSP_Stride) 2, output, (vDSP_Stride) 1, in_length >> 1);
#else
	unzip_complex_float (input, output, in_length >> 1);
#endif
		
	// If necessary replace the odd sample, and zero pad the input
		
	if (((HstFFT_UInt) 1 << log2n) > in_length)
	{
		realp[in_length >> 1] = temp;
		imagp[in_length >> 1] = 0.f;
		
		for (i = (in_length >> (HstFFT_UInt) 1) + 1; i < ((HstFFT_UInt) 1 << (log2n - (HstFFT_UInt) 1)); i++)
		{
			realp[i] = 0.f;
			imagp[i] = 0.f;
		}
	}
}

// N.B This routine specifically deals with unzipping float data into a double precision complex split format, thus removing the need for a temp memory location 

void hisstools_unzip_zero_fd (float *input, FFT_SPLIT_COMPLEX_D *output, HstFFT_UInt in_length, HstFFT_UInt log2n)
{
	HstFFT_UInt i;
	float temp = 0;
	
	double *realp = output->realp;
	double *imagp = output->imagp;
	
	// Check input length and get last value if in_length is odd
	
	if (((HstFFT_UInt) 1 << log2n) < in_length)
		in_length = (HstFFT_UInt) 1 << log2n;
	if (in_length & 1)
		temp = input[in_length - 1];	
	
	// Unzip an even number of samples
			
	for (i = 0; i < (in_length >> 1); i++)
	{
		*realp++ = *input++;
		*imagp++ = *input++;
	}
	
	// If necessary replace the odd sample, and zero pad the input
	
	if (((HstFFT_UInt) 1 << log2n) > in_length)
	{
		*realp++ = temp;
		*imagp++ = 0.;
		
		for (i = (in_length >> (HstFFT_UInt) 1) + 1; i < ((HstFFT_UInt) 1 << (log2n - (HstFFT_UInt) 1)); i++)
		{
			*realp++ = 0.;
			*imagp++ = 0.;
		}
	}
}

// Zip and Unzip

void hisstools_unzip_d (double *input, FFT_SPLIT_COMPLEX_D *output, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT	
	vDSP_ctozD((DOUBLE_COMPLEX *) input, (vDSP_Stride) 2, output, (vDSP_Stride) 1, (vDSP_Length) (1 << (log2n - 1)));
#else
	unzip_complex (input, output, (HstFFT_UInt) 1 << (log2n - (HstFFT_UInt) 1));
#endif
}

void hisstools_unzip_f (float *input, FFT_SPLIT_COMPLEX_F *output, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT	
	vDSP_ctoz((COMPLEX *) input, (vDSP_Stride) 2, output, (vDSP_Stride) 1, (vDSP_Length) (1 << (log2n - 1)));
#else
	unzip_complex_float (input, output, (HstFFT_UInt) 1 << (log2n - (HstFFT_UInt) 1));
#endif
}

void hisstools_zip_d (FFT_SPLIT_COMPLEX_D *input, double *output, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT	
	vDSP_ztocD(input, (vDSP_Stride) 1, (DOUBLE_COMPLEX *) output, (vDSP_Stride) 2, (vDSP_Length) (1 << (log2n - 1)));
#else
	zip_complex (input, output, (HstFFT_UInt) 1 << (log2n - (HstFFT_UInt) 1));
#endif
}

void hisstools_zip_f (FFT_SPLIT_COMPLEX_F *input, float *output, HstFFT_UInt log2n)
{
#ifdef USE_APPLE_FFT	
	vDSP_ztoc(input, (vDSP_Stride) 1, (COMPLEX *) output, (vDSP_Stride) 2, (vDSP_Length) (1 << (log2n - 1)));
#else
	zip_complex_float (input, output, (HstFFT_UInt) 1 << (log2n - (HstFFT_UInt) 1));
#endif
}

// Setup Create / Destory

FFT_SETUP_D hisstools_create_setup_d(HstFFT_UInt max_fft_log_2)
{
#ifdef USE_APPLE_FFT	
	return vDSP_create_fftsetupD(max_fft_log_2, FFT_RADIX2);
#else
	return create_setup(max_fft_log_2);
#endif
}

FFT_SETUP_F hisstools_create_setup_f(HstFFT_UInt max_fft_log_2)
{
#ifdef USE_APPLE_FFT	
	return vDSP_create_fftsetup(max_fft_log_2, FFT_RADIX2);
#else
	return create_setup_float(max_fft_log_2);
#endif
}

void hisstools_destroy_setup_d (FFT_SETUP_D setup)
{
	if (!setup)
		return;
		
#ifdef USE_APPLE_FFT	
	vDSP_destroy_fftsetupD(setup);
#else
	destroy_setup(setup);
#endif
}

void hisstools_destroy_setup_f (FFT_SETUP_F setup)
{
	if (!setup)
		return;
	
#ifdef USE_APPLE_FFT	
	vDSP_destroy_fftsetup(setup);
#else
	destroy_setup_float(setup);
#endif
}
