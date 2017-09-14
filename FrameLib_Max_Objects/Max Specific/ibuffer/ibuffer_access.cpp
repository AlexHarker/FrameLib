
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

//long IBuffer_SSE_Exists = 0;

void ibuffer_init()
{
    ps_buffer = gensym("buffer~");
    ps_ibuffer = gensym("ibuffer~");
    ps_none = gensym("none");
    ps_linear = gensym("linear");
    ps_bspline = gensym("bspline");
    ps_hermite = gensym("hermite");
    ps_lagrange = gensym("lagrange");
    
    //IBuffer_SSE_Exists = SSE2_check();
}

typedef SizedVector<4, SSEDouble> SSE4Double;

// Interpolators

template <class T>struct linear_interp
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

// Generic types of interpolation

template <class T, class U, class Ft> struct no_interp_reader
{
    no_interp_reader(const ibuffer_data& data, long chan) : fetch(data, chan) {}

    T operator()(intptr_t*& offsets, T fract)
    {
        float array[T::size];

        for (int i = 0; i < T::size; i++)
            array[i] = fetch(*offsets++);

        return U(array);
    }
    
    Ft fetch;
};

template <class T, class U, class Ft, typename Ip> struct interp_2samps
{
    interp_2samps(const ibuffer_data& data, long chan) : fetch(data, chan) {}

    T operator()(intptr_t*& offsets, T fract)
    {
        float array[T::size * 2];
        
        for (int i = 0; i < T::size; i++)
        {
            intptr_t offset = *offsets++;
            
            array[i] = fetch(offset);
            array[i + T::size] = fetch(offset + 1);
        }
        
        const T y0 = U(array);
        const T y1 = U(array + T::size);
        
        return interpolate(fract, y0, y1);
    }
    
    Ft fetch;
    Ip interpolate;
};

template <class T, class U, class Ft, typename Ip> struct interp_4samps
{
    interp_4samps(const ibuffer_data& data, long chan) : fetch(data, chan) {}
    
    T operator()(intptr_t*& offsets, T fract)
    {
        float array[T::size * 4];
        
        for (int i = 0; i < T::size; i++)
        {
            intptr_t offset = *offsets++;
            
            array[i] = fetch(offset - 1);
            array[i + T::size] = fetch(offset);
            array[i + T::size * 2] = fetch(offset + 1);
            array[i + T::size * 3] = fetch(offset + 2);
        }
        
        const T y0 = U(array);
        const T y1 = U(array + T::size);
        const T y2 = U(array + (T::size * 2));
        const T y3 = U(array + (T::size * 3));
        
        return interpolate(fract, y0, y1, y2, y3);
    }
    
    Ft fetch;
    Ip interpolate;
};

template <class T, class U, class Ft>
struct linear_reader : public interp_2samps<T, U, Ft, linear_interp<T> >
{
    linear_reader(const ibuffer_data& data, long chan) : interp_2samps<T, U, Ft, linear_interp<T> >(data, chan) {}
};

template <class T, class U, class Ft>
struct cubic_bspline_reader : public interp_4samps<T, U, Ft, cubic_bspline_interp<T> >
{
    cubic_bspline_reader(const ibuffer_data& data, long chan) : interp_4samps<T, U, Ft, cubic_bspline_interp<T> >(data, chan) {}
};


template <class T, class U, class Ft>
struct cubic_hermite_reader : public interp_4samps<T, U, Ft, cubic_hermite_interp<T> >
{
    cubic_hermite_reader(const ibuffer_data& data, long chan) : interp_4samps<T, U, Ft, cubic_hermite_interp<T> >(data, chan) {}
};

template <class T, class U, class Ft>
struct cubic_lagrange_reader : public interp_4samps<T, U, Ft, cubic_lagrange_interp<T> >
{
    cubic_lagrange_reader(const ibuffer_data& data, long chan) : interp_4samps<T, U, Ft, cubic_lagrange_interp<T> >(data, chan) {}
};

template <class T, class U, class Ft, template <class V, class W, class Ft2> class Ip>
void ibuffer_read_loop (const ibuffer_data& data, typename T::scalar_type *out, intptr_t *offsets, typename T::scalar_type *fracts, intptr_t n_samps, long chan, typename U::scalar_type mul)
{
    Ip<T, U, Ft> reader(data, chan);
    
    T *v_out = reinterpret_cast<T *>(out);
    T *v_fracts = reinterpret_cast<T *>(fracts);
    T scale = mul * reader.fetch.scale;
    
    for (intptr_t i = 0; i < (n_samps / T::size); i++)
        *v_out++ = scale * reader(offsets, *v_fracts++);
}

template <template <class T, class U, class Ft2> class Ip, class Ft>
void ibuffer_read(const ibuffer_data& data, float *out, intptr_t *offsets, float *fracts, intptr_t n_samps, long chan, float mul)
{
    intptr_t n_vsample = (n_samps / AVX256Float::size) * AVX256Float::size;
    
    ibuffer_read_loop<AVX256Float, AVX256Float, Ft, Ip>(data, out, offsets, fracts, n_vsample, chan, mul);
    ibuffer_read_loop<Scalar<float>, Scalar<float>, Ft, Ip>(data, out, offsets + n_vsample, fracts + n_vsample, n_samps - n_vsample, chan, mul);
}


template <template <class T, class U, class Ft2> class Ip, class Ft>
void ibuffer_read(const ibuffer_data& data, double *out, intptr_t *offsets, double *fracts, intptr_t n_samps, long chan, double mul)
{
    intptr_t n_vsample = (n_samps / AVX256Float::size) * AVX256Float::size;

    ibuffer_read_loop<AVX256Double, SSEFloat, Ft, Ip>(data, out, offsets, fracts, n_vsample, chan, mul);
    ibuffer_read_loop<Scalar<double>, Scalar<float>, Ft, Ip>(data, out, offsets + n_vsample, fracts + n_vsample, n_samps - n_vsample, chan, mul);
}

template <class T> void ibuffer_read(const ibuffer_data& data, T *out, intptr_t *offsets, T *fracts, intptr_t n_samps, long chan, T mul, InterpType interp)
{
    switch(interp)
    {
        case kInterpNone:
            
            switch(data.format)
            {
                case PCM_FLOAT:     ibuffer_read<no_interp_reader, fetch_float>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_16:    ibuffer_read<no_interp_reader, fetch_16bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_24:    ibuffer_read<no_interp_reader, fetch_24bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_32:    ibuffer_read<no_interp_reader, fetch_32bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
            }
            break;
            
        case kInterpLinear:

            switch(data.format)
            {
                case PCM_FLOAT:     ibuffer_read<linear_reader, fetch_float>(data, out, offsets, fracts, n_samps, chan, mul);   break;
                case PCM_INT_16:    ibuffer_read<linear_reader, fetch_16bit>(data, out, offsets, fracts, n_samps, chan, mul);   break;
                case PCM_INT_24:    ibuffer_read<linear_reader, fetch_24bit>(data, out, offsets, fracts, n_samps, chan, mul);   break;
                case PCM_INT_32:    ibuffer_read<linear_reader, fetch_32bit>(data, out, offsets, fracts, n_samps, chan, mul);   break;
            }
            break;
            
        case kInterpCubicHermite:
            
            switch(data.format)
            {
                case PCM_FLOAT:     ibuffer_read<cubic_hermite_reader, fetch_float>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_16:    ibuffer_read<cubic_hermite_reader, fetch_16bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_24:    ibuffer_read<cubic_hermite_reader, fetch_24bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_32:    ibuffer_read<cubic_hermite_reader, fetch_32bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
            }
            break;
            
        case kInterpCubicLagrange:
            
            switch(data.format)
            {
                case PCM_FLOAT:     ibuffer_read<cubic_lagrange_reader, fetch_float>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_16:    ibuffer_read<cubic_lagrange_reader, fetch_16bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_24:    ibuffer_read<cubic_lagrange_reader, fetch_24bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_32:    ibuffer_read<cubic_lagrange_reader, fetch_32bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
            }
            break;
            
        case kInterpCubicBSpline:
            
            switch(data.format)
            {
                case PCM_FLOAT:     ibuffer_read<cubic_bspline_reader, fetch_float>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_16:    ibuffer_read<cubic_bspline_reader, fetch_16bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_24:    ibuffer_read<cubic_bspline_reader, fetch_24bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
                case PCM_INT_32:    ibuffer_read<cubic_bspline_reader, fetch_32bit>(data, out, offsets, fracts, n_samps, chan, mul);    break;
            }
            break;
    }
}

void ibuffer_read(const ibuffer_data& data, double *out, intptr_t *offsets, double *fracts, intptr_t n_samps, long chan, double mul, InterpType interp)
{
    ibuffer_read<double>(data, out, offsets, fracts, n_samps, chan, mul, interp);
}

void ibuffer_read(const ibuffer_data& data, float *out, intptr_t *offsets, float *fracts, intptr_t n_samps, long chan, float mul, InterpType interp)
{
    ibuffer_read<float>(data, out, offsets, fracts, n_samps, chan, mul, interp);
}

template <class T, class Ft> void ibuffer_get_samps_loop(Ft fetch, T *out, intptr_t offset, intptr_t n_samps, bool reverse)
{
	if (reverse)
    {
        for (intptr_t i = n_samps - 1; i >= 0; i--)
            *out++ = fetch(offset + i);
    }
    else
    {
        for (intptr_t i = 0; i < n_samps; i++)
            *out++ = fetch(offset + i);
    }
}

template <class T> void ibuffer_get_samps(const ibuffer_data& data, T *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    switch(data.format)
    {
        case PCM_FLOAT:     ibuffer_get_samps_loop(fetch_float(data, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_16:    ibuffer_get_samps_loop(fetch_16bit(data, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_24:    ibuffer_get_samps_loop(fetch_24bit(data, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_32:    ibuffer_get_samps_loop(fetch_32bit(data, chan), out, offset, n_samps, reverse);     break;
    }
}

void ibuffer_get_samps(const ibuffer_data& data, float *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    ibuffer_get_samps<float>(data, out, offset, n_samps, chan, reverse);
}

void ibuffer_get_samps(const ibuffer_data& data, double *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    ibuffer_get_samps<double>(data, out, offset, n_samps, chan, reverse);
}
