
/*
 *  ibuffer_access.c
 *
 *	This file provides code for accessing and interpolating samplesfrom an ibuffer (or standard MSP buffer).
 *	Various kinds of interpolation are supported.
 *	All pointers used should be 16-byte aligned.
 *
 *	See the accompanying header file for more details.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "ibuffer.h"
#include "ibuffer_access.h"

t_symbol *ps_none;
t_symbol *ps_linear;
t_symbol *ps_bspline;
t_symbol *ps_hermite;
t_symbol *ps_lagrange;
t_symbol *ps_buffer;
t_symbol *ps_ibuffer;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// Fetch samples /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IBUFFER_FETCH_LOOP_UNROLL(x)			\
intptr_t i;									\
for (i = 0; i < n_samps >> 3; i++)				\
{x; x; x; x; x; x; x; x;}						\
for (i <<= 3; i < n_samps; i++)					\
{x;}											\
for (; i < ((n_samps + 3) >> 2) << 2; i++)		\
*out++ = 0;

static __inline void ibuffer_fetch_samps_float(float *out, float *samps, intptr_t *offsets, intptr_t n_samps)
{
    IBUFFER_FETCH_LOOP_UNROLL (*out++ = *(samps + *offsets++))
}

static __inline void ibuffer_fetch_samps_16(int32_t *out, int16_t *samps, intptr_t *offsets, intptr_t n_samps)
{
    IBUFFER_FETCH_LOOP_UNROLL (*out++ = (*(samps + *offsets++) << 16))
}

static __inline void ibuffer_fetch_samps_24(int32_t *out, int8_t *samps, intptr_t *offsets, intptr_t n_samps)
{
#if (TARGET_RT_LITTLE_ENDIAN)
    IBUFFER_FETCH_LOOP_UNROLL (*out++ = (*((int32_t *) ((samps - 1) + *offsets++)) & MASK_24_BIT))
#else
    IBUFFER_FETCH_LOOP_UNROLL (*out++ = (*((int32_t *) (samps + *offsets++)) & MASK_24_BIT))
#endif
}

static __inline void ibuffer_fetch_samps_32(int32_t *out, int32_t *samps, intptr_t *offsets, intptr_t n_samps)
{
    IBUFFER_FETCH_LOOP_UNROLL (*out++ = (*(samps + *offsets++)))
}

void ibuffer_fetch_samps(void *out, const ibuffer_data& data, intptr_t *offsets, intptr_t n_samps, long chan)
{
    switch (data.format)
    {
        case PCM_INT_16:
            ibuffer_fetch_samps_16((int32_t *) out, ((int16_t *) data.samples) + chan, offsets, n_samps);
            break;
        case PCM_INT_24:
            ibuffer_fetch_samps_24((int32_t *) out, (int8_t *) data.samples + (3 * chan), offsets, n_samps);
            break;
        case PCM_INT_32:
            ibuffer_fetch_samps_32((int32_t *) out, ((int32_t *) data.samples) + chan, offsets, n_samps);
            break;
        case PCM_FLOAT:
            ibuffer_fetch_samps_float((float *) out, (float *) data.samples + chan, offsets, n_samps);
            break;
    }
}

void ibuffer_fetch_samps_2(void **temp, const ibuffer_data& data, intptr_t *offsets, intptr_t n_samps, long chan)
{
    switch (data.format)
    {
        case PCM_INT_16:
            ibuffer_fetch_samps_16((int32_t *) temp[0], ((int16_t *) data.samples) + chan, offsets, n_samps);
            ibuffer_fetch_samps_16((int32_t *) temp[1], ((int16_t *) data.samples) + chan + data.n_chans, offsets, n_samps);
            break;
        case PCM_INT_24:
            ibuffer_fetch_samps_24((int32_t *) temp[0], ((int8_t *) data.samples) + (3 * chan), offsets, n_samps);
            ibuffer_fetch_samps_24((int32_t *) temp[1], ((int8_t *) data.samples) + (3 * (chan + data.n_chans)), offsets, n_samps);
            break;
        case PCM_INT_32:
            ibuffer_fetch_samps_32((int32_t *) temp[0], ((int32_t *) data.samples) + chan, offsets, n_samps);
            ibuffer_fetch_samps_32((int32_t *) temp[1], ((int32_t *) data.samples) + chan + data.n_chans, offsets, n_samps);
            break;
        case PCM_FLOAT:
            ibuffer_fetch_samps_float((float *) temp[0], ((float *) data.samples) + chan, offsets, n_samps);
            ibuffer_fetch_samps_float((float *) temp[1], ((float *) data.samples) + chan + data.n_chans, offsets, n_samps);
            break;
    }
}

void ibuffer_fetch_samps_4 (void **temp, const ibuffer_data& data, intptr_t *offsets, intptr_t n_samps, long chan)
{
    switch (data.format)
    {
        case PCM_INT_16:
            ibuffer_fetch_samps_16((int32_t *) temp[0], ((int16_t *) data.samples) + chan - data.n_chans, offsets, n_samps);
            ibuffer_fetch_samps_16((int32_t *) temp[1], ((int16_t *) data.samples) + chan, offsets, n_samps);
            ibuffer_fetch_samps_16((int32_t *) temp[2], ((int16_t *) data.samples) + chan + data.n_chans, offsets, n_samps);
            ibuffer_fetch_samps_16((int32_t *) temp[3], ((int16_t *) data.samples) + chan + (data.n_chans << 1), offsets, n_samps);
            break;
        case PCM_INT_24:
            ibuffer_fetch_samps_24((int32_t *) temp[0], ((int8_t *) data.samples) + (3 * (chan - data.n_chans)), offsets, n_samps);
            ibuffer_fetch_samps_24((int32_t *) temp[1], ((int8_t *) data.samples) + (3 * chan), offsets, n_samps);
            ibuffer_fetch_samps_24((int32_t *) temp[2], ((int8_t *) data.samples) + (3 * (chan + data.n_chans)), offsets, n_samps);
            ibuffer_fetch_samps_24((int32_t *) temp[3], ((int8_t *) data.samples) + (3 * (chan + (data.n_chans << 1))), offsets, n_samps);
            break;
        case PCM_INT_32:
            ibuffer_fetch_samps_32((int32_t *) temp[0], ((int32_t *) data.samples) + chan - data.n_chans, offsets, n_samps);
            ibuffer_fetch_samps_32((int32_t *) temp[1], ((int32_t *) data.samples) + chan, offsets, n_samps);
            ibuffer_fetch_samps_32((int32_t *) temp[2], ((int32_t *) data.samples) + chan + data.n_chans, offsets, n_samps);
            ibuffer_fetch_samps_32((int32_t *) temp[3], ((int32_t *) data.samples) + chan + (data.n_chans << 1), offsets, n_samps);
            break;
        case PCM_FLOAT:
            ibuffer_fetch_samps_float((float *) temp[0], ((float *) data.samples) + chan - data.n_chans, offsets, n_samps);
            ibuffer_fetch_samps_float((float *) temp[1], ((float *) data.samples) + chan, offsets, n_samps);
            ibuffer_fetch_samps_float((float *) temp[2], ((float *) data.samples) + chan + data.n_chans, offsets, n_samps);
            ibuffer_fetch_samps_float((float *) temp[3], ((float *) data.samples) + chan + (data.n_chans << 1), offsets, n_samps);
            break;
    }
}

// Interpolators

template <class T> struct linear_interp
{
    T operator()(const T& x, const T& y0, const T& y1) { return  (y0 + x * ((y1 - y0))); }
};

template <class T> struct cubic_hermite_interp
{
    cubic_hermite_interp() : _5div2(2.5), _3div2(1.5), _1div2(0.5) {}

    T operator()(const T& x, const T& y0, const T& y1, const T& y2, const T& y3)
    {
        const T c0 = y1;
        const T c1 = _1div2 * (y2 - y0);
        const T c2 = y0 - _5div2 * y1 + y2 + y2 - _1div2 * y3;
        const T c3 = _1div2 * (y3 - y0) + _3div2 * (y1 - y2);
        
        return (((c3 * x + c2) * x + c1) * x + c0);
    }
    
    const T _5div2;
    const T _3div2;
    const T _1div2;
};

template <class T> struct cubic_lagrange_interp
{
    cubic_lagrange_interp() : _1div3(1.0/3.0), _1div6(1.0/6.0), _1div2(0.5) {}
    
    T operator()(const T& x, const T& y0, const T& y1, const T& y2, const T& y3)
    {
        const T c0 = y1;
        const T c1 = y2 - _1div3 * y0 - _1div2 * y1 - _1div6 * y3;
        const T c2 = _1div2 * (y0 + y2) - y1;
        const T c3 = _1div6 * (y3 - y0) + _1div2 * (y1 - y2);
        
        return (((c3 * x + c2) * x + c1) * x + c0);
    }
    
    const T _1div3;
    const T _1div6;
    const T _1div2;
};

template <class T> struct cubic_bspline_interp
{
    cubic_bspline_interp() : _2div3(2.0/3.0), _1div6(1.0/6.0), _1div2(0.5) {}

    T operator()(const T& x, const T& y0, const T& y1, const T& y2, const T& y3)
    {
        const T y0py2 = y0 + y2;
        const T c0 = _1div6 * y0py2 + _2div3 * y1;
        const T c1 = _1div2 * (y2 - y0);
        const T c2 = _1div2 * y0py2 - y1;
        const T c3 = _1div2 * (y1 - y2) + _1div6 * (y3 - y0);
        
        return (((c3 * x + c2) * x + c1) * x + c0);
    }
    
    const T _2div3;
    const T _1div6;
    const T _1div2;
};

// Generic ypes of interpolation

template <class T, class U, typename Ip> void ibuffer_4point_interp(U *out,  void **inbuffers, U *fracts, intptr_t n_samps, typename U::scalar_type mul, Ip ip)
{
    T *in1 = (T *) inbuffers[0];
    T *in2 = (T *) inbuffers[1];
    T *in3 = (T *) inbuffers[2];
    T *in4 = (T *) inbuffers[3];
    
    U scale = mul;
    
    for (intptr_t i = 0; i < (n_samps / U::size); i++)
        *out++ = scale * ip(*fracts++, static_cast<U>(*in1++), static_cast<U>(*in2++), static_cast<U>(*in3++), static_cast<U>(*in4++));
}

template <int size> void get_4samps(float *array, const ibuffer_data& data, long chan, intptr_t offset)
{
    const float *samples = ((float *) data.samples) + chan + ((offset) * data.n_chans);
    
    array[0] = *(samples - data.n_chans);
    array[size] = *(samples);
    array[size * 2] = *(samples + data.n_chans);
    array[size * 3] = *(samples + 2 * data.n_chans);
}

template <class T, class U, typename Ip> T interp_4samps(const ibuffer_data& data, long chan, intptr_t*& offsets, T fract, Ip ip)
{
    float array[T::size * 4];
    
    for (int i = 0; i < T::size; i++)
        get_4samps<T::size>(array + i, data, chan, *offsets++);
    
    const T y0 = U(array);
    const T y1 = U(array + T::size);
    const T y2 = U(array + (T::size * 2));
    const T y3 = U(array + (T::size * 3));
    
    return ip(fract, y0, y1, y2, y3);
}

template <int size> void get_2samps(float *array, const ibuffer_data& data, long chan, intptr_t offset)
{
    const float *samples = ((float *) data.samples) + chan + ((offset) * data.n_chans);
    
    array[0] = *(samples );
    array[size] = *(samples + data.n_chans);
}

template <class T, class U, typename Ip> T interp_2samps(const ibuffer_data& data, long chan, intptr_t*& offsets, T fract, Ip ip)
{
    float array[T::size * 2];
    
    for (int i = 0; i < T::size; i++)
        get_2samps<T::size>(array + i, data, chan, *offsets++);
    
    const T y0 = U(array);
    const T y1 = U(array + T::size);

    return ip(fract, y0, y1);
}


template <class T, class U, typename Ip> void ibuffer_4point_interp(const ibuffer_data& data, U *out, intptr_t *offsets, U *fracts, intptr_t n_samps, long chan, typename U::scalar_type mul, Ip ip)
{
    U scale = mul;
    
    for (intptr_t i = 0; i < (n_samps / U::size); i++)
        *out++ = scale * interp_4samps<U, T>(data, chan, offsets, *fracts++, ip);
}

template <class T, class U, typename Ip> void ibuffer_2point_interp(const ibuffer_data& data, U *out, intptr_t *offsets, U *fracts, intptr_t n_samps, long chan, typename U::scalar_type mul, Ip ip)
{
    U scale = mul;
    
    for (intptr_t i = 0; i < (n_samps / U::size); i++)
        *out++ = scale * interp_2samps<U, T>(data, chan, offsets, *fracts++, ip);
}

template <class T, class V, class U, typename Ip> void ibuffer_simd_4point(const ibuffer_data& data, U *out, intptr_t *offsets, U *fracts, void **temp, intptr_t n_samps, long chan, typename U::scalar_type mul, Ip ip)
{
    ibuffer_fetch_samps_4(temp, data, offsets, n_samps, chan);
    
    if (data.format == PCM_FLOAT)
        ibuffer_4point_interp<V>(out, temp, fracts, n_samps, mul, ip);
    else
        ibuffer_4point_interp<T>(out, temp, fracts, n_samps, mul * TWO_POW_31_RECIP_DOUBLE, ip);
}

template <class T, typename Ip> void ibuffer_scalar_4point(const ibuffer_data& data, T *out, intptr_t *offsets, T *fracts, intptr_t n_samps, long chan, T mul, Ip ip)
{
    for (intptr_t i = 0; i < n_samps; i++)
    {
        intptr_t offset = offsets[i];
        
        const T y0 = ibuffer_float_get_samp(data, offset - 1, chan);
        const T y1 = ibuffer_float_get_samp(data, offset, chan);
        const T y2 = ibuffer_float_get_samp(data, offset + 1, chan);
        const T y3 = ibuffer_float_get_samp(data, offset + 2, chan);

        *out++ = mul * ip(*fracts++, y0, y1, y2, y3);
    }
}

template <class T, class U, typename Ip> void ibuffer_2point_interp(U *out,  void **inbuffers, U *fracts, intptr_t n_samps, typename U::scalar_type mul, Ip ip)
{
    T *in1 = (T *) inbuffers[0];
    T *in2 = (T *) inbuffers[1];
    
    U scale = mul;
    
    for (intptr_t i = 0; i < (n_samps / U::size); i++)
        *out++ = scale * ip(*fracts++, static_cast<U>(*in1++), static_cast<U>(*in2++));
}

template <class T, class V, class U, typename Ip> void ibuffer_simd_2point(const ibuffer_data& data, U *out, intptr_t *offsets, U *fracts, void **temp, intptr_t n_samps, long chan, typename U::scalar_type mul, Ip ip)
{
    ibuffer_fetch_samps_2(temp, data, offsets, n_samps, chan);
    
    if (data.format == PCM_FLOAT)
        ibuffer_2point_interp<V>(out, temp, fracts, n_samps, mul, ip);
    else
        ibuffer_2point_interp<T>(out, temp, fracts, n_samps, mul * TWO_POW_31_RECIP_DOUBLE, ip);
}

template <class T, typename Ip> void ibuffer_scalar_2point(const ibuffer_data& data, T *out, intptr_t *offsets, T *fracts, intptr_t n_samps, long chan, T mul, Ip ip)
{
    for (intptr_t i = 0; i < n_samps; i++)
    {
        intptr_t offset = offsets[i];
        
        const T y0 = ibuffer_float_get_samp(data, offset - 1, chan);
        const T y1 = ibuffer_float_get_samp(data, offset, chan);
        
        *out++ = mul * ip(*fracts++, y0, y1);
    }
}

// SIMD


void ibuffer_float_samps_simd_linear(const ibuffer_data& data, SSEFloat *out, intptr_t *offsets, SSEFloat *fracts, void **temp, intptr_t n_samps, long chan, float mul)
{
    ibuffer_simd_2point<SSEInt32, SSEFloat>(data, out, offsets, fracts, temp, n_samps, chan, mul, linear_interp<SSEFloat>());
}

void ibuffer_double_samps_simd_linear(const ibuffer_data& data, SSE4Double *out, intptr_t *offsets, SSE4Double *fracts, intptr_t n_samps,long chan, double mul)
{
    //ibuffer_simd_2point<SSEInt32, SSEFloat>(data, out, offsets, fracts, temp, n_samps, chan, mul, linear_interp<SSE4Double>());
    //ibuffer_2point_interp<SSEFloat>(data, out, offsets, fracts, n_samps, chan, mul, linear_interp<AVX256Double>());
    ibuffer_2point_interp<SSEFloat>(data, out, offsets, fracts, n_samps, chan, mul, linear_interp<SSE4Double>());

}

void ibuffer_float_samps_simd_cubic_bspline(const ibuffer_data& data,  SSEFloat *out, intptr_t *offsets, SSEFloat *fracts, void **temp, intptr_t n_samps, long chan, float mul)
{
    ibuffer_simd_4point<SSEInt32, SSEFloat>(data, out, offsets, fracts, temp, n_samps, chan, mul, cubic_bspline_interp<SSEFloat>());
}

void ibuffer_double_samps_simd_cubic_bspline(const ibuffer_data& data, AVX256Double *out, intptr_t *offsets, AVX256Double *fracts, intptr_t n_samps, long chan, double mul)
{
    //ibuffer_simd_4point<SSEInt32, SSEFloat>(data, out, offsets, fracts, temp, n_samps, chan, mul, cubic_bspline_interp<SSE4Double>());
    ibuffer_4point_interp<SSEFloat>(data, out, offsets, fracts, n_samps, chan, mul, cubic_bspline_interp<AVX256Double>());
}

void ibuffer_float_samps_simd_cubic_lagrange(const ibuffer_data& data, SSEFloat *out, intptr_t *offsets, SSEFloat *fracts, void **temp, intptr_t n_samps, long chan, float mul)
{
    ibuffer_simd_4point<SSEInt32, SSEFloat>(data, out, offsets, fracts, temp, n_samps, chan, mul, cubic_lagrange_interp<SSEFloat>());
}

void ibuffer_double_samps_simd_cubic_lagrange(const ibuffer_data& data, AVX256Double *out, intptr_t *offsets, AVX256Double *fracts, intptr_t n_samps, long chan, double mul)
{
    //ibuffer_simd_4point<SSEInt32, SSEFloat>(data, out, offsets, fracts, temp, n_samps, chan, mul, cubic_lagrange_interp<SSE4Double>());
    ibuffer_4point_interp<SSEFloat>(data, out, offsets, fracts, n_samps, chan, mul, cubic_lagrange_interp<AVX256Double>());

}

void ibuffer_float_samps_simd_cubic_hermite(const ibuffer_data& data, SSEFloat *out, intptr_t *offsets, SSEFloat *fracts, void **temp, intptr_t n_samps, long chan, float mul)
{
    //ibuffer_simd_4point<SSEInt32, SSEFloat>(data, out, offsets, fracts, temp, n_samps, chan, mul, cubic_hermite_interp<SSEFloat>());
    ibuffer_4point_interp<SSEFloat>(data, out, offsets, fracts, n_samps, chan, mul, cubic_hermite_interp<SSEFloat>());

}

void ibuffer_double_samps_simd_cubic_hermite(const ibuffer_data& data, AVX256Double *out, intptr_t *offsets, AVX256Double *fracts, intptr_t n_samps, long chan, double mul)
{
    //ibuffer_simd_4point<SSEInt32, SSEFloat>(data, out, offsets, fracts, temp, n_samps, chan, mul, cubic_hermite_interp<AVX256Double>());
    ibuffer_4point_interp<SSEFloat>(data, out, offsets, fracts, n_samps, chan, mul, cubic_hermite_interp<AVX256Double>());
}

// SCALAR CODE

void ibuffer_float_samps_scalar_linear(const ibuffer_data& data, float *out, intptr_t *offsets, float *fracts, intptr_t n_samps, long chan, float mul)
{
    ibuffer_scalar_2point(data, out, offsets, fracts, n_samps, chan, mul, linear_interp<float>());
}

void ibuffer_double_samps_scalar_linear(const ibuffer_data& data, double *out, intptr_t *offsets, double *fracts, intptr_t n_samps, long chan, double mul)
{
    ibuffer_scalar_2point(data, out, offsets, fracts, n_samps, chan, mul, linear_interp<double>());
}

void ibuffer_float_samps_scalar_cubic_bspline(const ibuffer_data& data, float *out, intptr_t *offsets, float *fracts, intptr_t n_samps, long chan, float mul)
{
    ibuffer_scalar_4point(data, out, offsets, fracts, n_samps, chan, mul, cubic_bspline_interp<float>());
}

void ibuffer_double_samps_scalar_cubic_bspline(const ibuffer_data& data, double *out, intptr_t *offsets, double *fracts, intptr_t n_samps, long chan, double mul)
{
    ibuffer_scalar_4point(data, out, offsets, fracts, n_samps, chan, mul, cubic_bspline_interp<double>());
}

void ibuffer_float_samps_scalar_cubic_hermite(const ibuffer_data& data, float *out, intptr_t *offsets, float *fracts, intptr_t n_samps, long chan, float mul)
{
    ibuffer_scalar_4point(data, out, offsets, fracts, n_samps, chan, mul, cubic_hermite_interp<float>());
}

void ibuffer_double_samps_scalar_cubic_hermite(const ibuffer_data& data, double *out, intptr_t *offsets, double *fracts, intptr_t n_samps, long chan, double mul)
{
    ibuffer_scalar_4point(data, out, offsets, fracts, n_samps, chan, mul, cubic_hermite_interp<double>());
}

void ibuffer_float_samps_scalar_cubic_lagrange(const ibuffer_data& data, float *out, intptr_t *offsets, float *fracts, intptr_t n_samps, long chan, float mul)
{
    ibuffer_scalar_4point(data, out, offsets, fracts, n_samps, chan, mul, cubic_lagrange_interp<float>());
}

void ibuffer_double_samps_scalar_cubic_lagrange(const ibuffer_data& data, double *out, intptr_t *offsets, double *fracts, intptr_t n_samps, long chan, double mul)
{
    ibuffer_scalar_4point(data, out, offsets, fracts, n_samps, chan, mul, cubic_lagrange_interp<double>());
}


long IBuffer_SSE_Exists = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// Symbols and init (for symbols) /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ibuffer_init ()
{
	ps_buffer = gensym("buffer~");
	ps_ibuffer = gensym("ibuffer~");
	ps_none = gensym("none");
	ps_linear = gensym("linear");
	ps_bspline = gensym("bspline");
	ps_hermite = gensym("hermite");
	ps_lagrange = gensym("lagrange");
	
	IBuffer_SSE_Exists = SSE2_check();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Convert from 32 bit integer format to 32 bit float ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef __APPLE__

static __inline void convert_and_scale_int32_to_float (float *out, intptr_t n_samps)
{
	vFloat scale = {TWO_POW_31_RECIP, TWO_POW_31_RECIP, TWO_POW_31_RECIP, TWO_POW_31_RECIP};
	
	vSInt32	*ivec_ptr = (vSInt32 *) out;
	vFloat *fvec_ptr = (vFloat *) out;
	int32_t *temp_ptr;
		
	intptr_t i;
	
	// Do vectors
	
	for (i = 0; i < n_samps >> 2; i++)
		*fvec_ptr++ = F32_VEC_MUL_OP (scale, F32_VEC_FROM_I32 (*ivec_ptr++));
	
	// Clean up with scalars
	
	for (temp_ptr = (int32_t *) ivec_ptr, out = (float *) fvec_ptr, i <<= 2; i < n_samps; i++)
		*out++ = (float) *temp_ptr++ * TWO_POW_31_RECIP;	
}

#else

static __inline void convert_and_scale_int32_to_float (float *out, intptr_t n_samps)
{
	vFloat scale = {TWO_POW_31_RECIP, TWO_POW_31_RECIP, TWO_POW_31_RECIP, TWO_POW_31_RECIP};
	
	vSInt32	*ivec_ptr;
	vFloat *fvec_ptr ;
	int32_t *temp_ptr = (int32_t *) out;
	
	intptr_t start_offset = (16 - ((intptr_t) out % 16)) >> 2;
	intptr_t i;
		
	if (start_offset == 4)
		start_offset = 0;
	
	// This should avoid executing any SSE instructions - but it may not be safe - check later...
	
	if (!IBuffer_SSE_Exists)
		start_offset = n_samps; 
	
	// Start with scalars
	
	for (i = 0; i < start_offset && i < n_samps; i++)
		*out++ = (float) *temp_ptr++ * TWO_POW_31_RECIP;	
	
	// Do vectors
	
	for (ivec_ptr = (vSInt32 *) temp_ptr, fvec_ptr = (vFloat *) out; i < n_samps - 3; i += 4)
		*fvec_ptr++ = F32_VEC_MUL_OP (scale, F32_VEC_FROM_I32 (*ivec_ptr++));
	
	// Clean up with scalars
	
	for (temp_ptr = (int32_t *) ivec_ptr, out = (float *) fvec_ptr; i < n_samps; i++)
		*out++ = (float) *temp_ptr++ * TWO_POW_31_RECIP;	
}

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Get multiple consecutive samples ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ibuffer_get_samps_16 (void *samps, float *out, intptr_t offset, intptr_t n_samps, long n_chans, long chan)
{
	int16_t *sampschan = ((int16_t *) samps) + chan + (offset * n_chans);
	int32_t *temp_ptr = (int32_t *) out;
	intptr_t i;
	
	for (i = 0; i < n_samps; i++)
		*temp_ptr++ = (*(sampschan + (i * n_chans)) << 16) & MASK_16_BIT;
		
	convert_and_scale_int32_to_float (out, n_samps);
}

void ibuffer_get_samps_24 (void *samps, float *out, intptr_t offset, intptr_t n_samps, long n_chans, long chan)
{	
	int8_t *sampschan = ((int8_t *) samps) + (3 * (chan + (offset * n_chans))) - 1;
	int32_t *temp_ptr = (int32_t *) out;
	intptr_t i;
	
	for (i = 0; i < n_samps; i++)
		*temp_ptr++ = *((int32_t *)(sampschan + (i * 3 * n_chans))) & MASK_24_BIT;
		
	convert_and_scale_int32_to_float (out, n_samps);
}

void ibuffer_get_samps_32 (void *samps, float *out, intptr_t offset, intptr_t n_samps, long n_chans, long chan)
{
	int32_t *sampschan = ((int32_t *) samps) + chan + (offset * n_chans);
	int32_t	*temp_ptr = (int32_t *) out;
	intptr_t i;
		
	for (i = 0; i < n_samps; i++)
		*temp_ptr++ = *(sampschan + (i * n_chans));
	
	convert_and_scale_int32_to_float (out, n_samps);
}

void ibuffer_get_samps_float (void *samps, float *out, intptr_t offset, intptr_t n_samps, long n_chans, long chan)
{
	float *sampschan = ((float *) samps) + chan + (offset * n_chans);
	intptr_t i;
	
	for (i = 0; i < n_samps; i++)
		*out++ = *(sampschan + (i * n_chans));
}

void ibuffer_get_samps(void *samps, float *out, intptr_t offset, intptr_t n_samps, long n_chans, long chan, long format)
{
	switch (format)
	{
		case PCM_INT_16:
			ibuffer_get_samps_16 (samps, out, offset, n_samps, n_chans, chan);
			break;
			
		case PCM_INT_24:
			ibuffer_get_samps_24 (samps, out, offset, n_samps, n_chans, chan);
			break;
			
		case PCM_INT_32:
			ibuffer_get_samps_32 (samps, out, offset, n_samps, n_chans, chan);
			break;
		
		case PCM_FLOAT:
			ibuffer_get_samps_float (samps, out, offset, n_samps, n_chans, chan);
			break;			
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Get multiple consecutive samples in reverse ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ibuffer_get_samps_rev_16 (void *samps, float *out, intptr_t offset, intptr_t n_samps, long n_chans, long chan)
{
	int16_t *sampschan = ((int16_t *) samps) + chan + (offset * n_chans);
	int32_t *temp_ptr = (int32_t *) out;
	intptr_t i;
	
	for (i = n_samps - 1; i >= 0; i--)
		*temp_ptr++ = (*(sampschan + (i * n_chans)) << 16) & MASK_16_BIT;
	
	convert_and_scale_int32_to_float (out, n_samps);
}

void ibuffer_get_samps_rev_24 (void *samps, float *out, intptr_t offset, intptr_t n_samps, long n_chans, long chan)
{
	int8_t *sampschan = ((int8_t *) samps) + (3 * (chan + (offset * n_chans))) - 1;
	int32_t *temp_ptr = (int32_t *) out;
	intptr_t i;
	
	for (i = n_samps - 1; i >= 0; i--)
		*temp_ptr++ = *((int32_t *)(sampschan + (i * 3 * n_chans)))  & MASK_24_BIT;;
		
	convert_and_scale_int32_to_float (out, n_samps);
}

void ibuffer_get_samps_rev_32 (void *samps, float *out, intptr_t offset, intptr_t n_samps, long n_chans, long chan)
{
	int32_t *sampschan = ((int32_t *) samps) + chan + + (offset * n_chans);
	int32_t *temp_ptr = (int32_t *) out;
	intptr_t i;
	
	for (i = 0; i < n_samps; i++)
		*temp_ptr++ = *(sampschan + (i * n_chans));
	
	convert_and_scale_int32_to_float (out, n_samps);
}

void ibuffer_get_samps_rev_float (void *samps, float *out, intptr_t offset, intptr_t n_samps, long n_chans, long chan)
{
	float *sampschan = ((float *) samps) + chan + (offset * n_chans);
	intptr_t i;
	
	for (i = n_samps - 1; i >= 0; i--)
		*out++ = *(sampschan + (i * n_chans));
}

void ibuffer_get_samps_rev (void *samps, float *out, intptr_t offset, intptr_t n_samps, long n_chans, long chan, long format)
{
	switch (format)
	{
		case PCM_INT_16:
			ibuffer_get_samps_rev_16 (samps, out, offset, n_samps, n_chans, chan);
			break;
			
		case PCM_INT_24:
			ibuffer_get_samps_rev_24 (samps, out, offset, n_samps, n_chans, chan);
			break;
			
		case PCM_INT_32:
			ibuffer_get_samps_rev_32 (samps, out, offset, n_samps, n_chans, chan);
			break;
		
		case PCM_FLOAT:
			ibuffer_get_samps_rev_float (samps, out, offset, n_samps, n_chans, chan);
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Get samples with no interpolation

void ibuffer_float_samps_simd_nointerp (const ibuffer_data& data, vFloat *out, intptr_t *offsets, intptr_t n_samps, long chan, float mul)
{
	vFloat scale = float2vector ((data.format == PCM_FLOAT) ? mul : (mul * TWO_POW_31_RECIP));
	vSInt32 *int_vec = (vSInt32 *) out;
	intptr_t i;	

	ibuffer_fetch_samps((void *) out, data, offsets, n_samps, chan);
	
	if (data.format == PCM_FLOAT)
	{
		for (i = 0; i < ((n_samps + 3) >> 2); i++, out++)
			*out = F32_VEC_MUL_OP (scale, *out);
	}
	else 
	{
		for (i = 0; i < (n_samps + 3) >> 2; i++)
			*out++ = F32_VEC_MUL_OP (scale, F32_VEC_FROM_I32 (*int_vec++));
	}
}

void ibuffer_float_samps_scalar_nointerp (const ibuffer_data& data, float *out, intptr_t *offsets, intptr_t n_samps, long chan, float mul)
{
	intptr_t i;	
		
	for (i = 0; i < n_samps; i++)
		*out++ = mul * ibuffer_float_get_samp(data, offsets[i], chan);
}

void ibuffer_double_samps_simd_nointerp (const ibuffer_data& data, vDouble *out, intptr_t *offsets, intptr_t n_samps, long chan, double mul)
{
	vDouble scale = double2vector ((data.format == PCM_FLOAT) ? mul : (mul * TWO_POW_31_RECIP));
	vFloat *float_vec = (vFloat *) ((double *) out + (((n_samps + 3) >> 2) << 1));
	vFloat float_temp;
	vSInt32 *int_vec = (vSInt32 *) float_vec;
	vSInt32 int_temp;
	intptr_t i;	
	
	ibuffer_fetch_samps((void *) float_vec, data, offsets, n_samps, chan);
	
	if (data.format == PCM_FLOAT)
	{
		for (i = 0; i < ((n_samps + 3) >> 2); i++)
		{
			float_temp = *float_vec++;
			*out++ = F64_VEC_MUL_OP (scale, F64_VEC_FROM_F32(float_temp));
			*out++ = F64_VEC_MUL_OP (scale, F64_VEC_FROM_F32(F32_VEC_MOVE_HI(float_temp, float_temp)));
		}
	}
	else 
	{
		for (i = 0; i < ((n_samps + 3) >> 2); i++)
		{
			int_temp = *int_vec++;
			*out++ = F64_VEC_MUL_OP(scale, F64_VEC_FROM_I32(int_temp));
			*out++ = F64_VEC_MUL_OP(scale, F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(int_temp, 0xE)));
		}
	}
}

void ibuffer_double_samps_scalar_nointerp (const ibuffer_data& data, double *out, intptr_t *offsets, intptr_t n_samps, long chan, double mul)
{
	intptr_t i;	
	
	for (i = 0; i < n_samps; i++)
		*out++ = mul * ibuffer_double_get_samp(data, offsets[i], chan);
}
