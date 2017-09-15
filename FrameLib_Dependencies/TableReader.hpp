
#ifndef TABLEREADER_H
#define TABLEREADER_H

#include "Interpolation.hpp"
#include "SIMDSupport.hpp"

// Generic types of interpolation

template <class T, class U, class Ft> struct no_interp_reader
{
    no_interp_reader(Ft fetcher) : fetch(fetcher) {}
    
    T operator()(intptr_t*& offsets, T fract)
    {
        typename U::scalar_type array[T::size];
        
        for (int i = 0; i < T::size; i++)
            array[i] = fetch(*offsets++);
        
        return U(array);
    }
    
    Ft fetch;
};

template <class T, class U, class Ft, typename Ip> struct interp_2_reader
{
    interp_2_reader(Ft fetcher) : fetch(fetcher) {}
    
    T operator()(intptr_t*& offsets, T fract)
    {
        typename U::scalar_type array[T::size * 2];
        
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

template <class T, class U, class Ft, typename Ip> struct interp_4_reader
{
    interp_4_reader(Ft fetcher) : fetch(fetcher) {}
    
    T operator()(intptr_t*& offsets, T fract)
    {
        typename U::scalar_type array[T::size * 4];
        
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
struct linear_reader : public interp_2_reader<T, U, Ft, linear_interp<T> >
{
    linear_reader(Ft fetcher) : interp_2_reader<T, U, Ft, linear_interp<T> >(fetcher) {}
};

template <class T, class U, class Ft>
struct cubic_bspline_reader : public interp_4_reader<T, U, Ft, cubic_bspline_interp<T> >
{
    cubic_bspline_reader(Ft fetcher) : interp_4_reader<T, U, Ft, cubic_bspline_interp<T> >(fetcher) {}
};

template <class T, class U, class Ft>
struct cubic_hermite_reader : public interp_4_reader<T, U, Ft, cubic_hermite_interp<T> >
{
    cubic_hermite_reader(Ft fetcher) : interp_4_reader<T, U, Ft, cubic_hermite_interp<T> >(fetcher) {}
};

template <class T, class U, class Ft>
struct cubic_lagrange_reader : public interp_4_reader<T, U, Ft, cubic_lagrange_interp<T> >
{
    cubic_lagrange_reader(Ft fetcher) : interp_4_reader<T, U, Ft, cubic_lagrange_interp<T> >(fetcher) {}
};

template <class T, class U, class Ft, template <class V, class W, class Ft2> class Ip>
void table_read_loop(Ft fetcher, typename T::scalar_type *out, intptr_t *offsets, typename T::scalar_type *fracts, intptr_t n_samps, typename U::scalar_type mul)
{
    Ip<T, U, Ft> reader(fetcher);
    
    T *v_out = reinterpret_cast<T *>(out);
    T *v_fracts = reinterpret_cast<T *>(fracts);
    T scale = mul * reader.fetch.scale;
    
    for (intptr_t i = 0; i < (n_samps / T::size); i++)
        *v_out++ = scale * reader(offsets, *v_fracts++);
}

template <template <class T, class U, class Ft2> class Ip, class Ft>
void table_read(Ft fetcher, float *out, intptr_t *offsets, float *fracts, intptr_t n_samps, float mul)
{
    intptr_t n_vsample = (n_samps / AVX256Float::size) * AVX256Float::size;
    
    table_read_loop<AVX256Float, AVX256Int32, Ft, Ip>(fetcher, out, offsets, fracts, n_vsample, mul);
    table_read_loop<Scalar<float>, Scalar<float>, Ft, Ip>(fetcher, out, offsets + n_vsample, fracts + n_vsample, n_samps - n_vsample, mul);
}

template <template <class T, class U, class Ft2> class Ip, class Ft>
void table_read(Ft fetcher, double *out, intptr_t *offsets, double *fracts, intptr_t n_samps, double mul)
{
    intptr_t n_vsample = (n_samps / AVX256Float::size) * AVX256Float::size;
    
    table_read_loop<AVX256Double, SSEFloat, Ft, Ip>(fetcher, out, offsets, fracts, n_vsample, mul);
    table_read_loop<Scalar<double>, Scalar<float>, Ft, Ip>(fetcher, out, offsets + n_vsample, fracts + n_vsample, n_samps - n_vsample, mul);
}
/*
 template <template <class T, class U, class Ft2> class Ip>
 void ibuffer_read_float(fetch_float fetcher, float *out, intptr_t *offsets, float *fracts, intptr_t n_samps, float mul)
 {
 intptr_t n_vsample = (n_samps / AVX256Float::size) * AVX256Float::size;
 
 ibuffer_read_loop<AVX256Float, AVX256Float, fetch_float, Ip>(fetcher, out, offsets, fracts, n_vsample, mul);
 ibuffer_read_loop<Scalar<float>, Scalar<float>, fetch_float, Ip>(fetcher, out, offsets + n_vsample, fracts + n_vsample, n_samps - n_vsample, mul);
 }
 
 
 template <template <class T, class U, class Ft2> class Ip>
 void ibuffer_read_float(fetch_float fetcher, double *out, intptr_t *offsets, double *fracts, intptr_t n_samps, double mul)
 {
 intptr_t n_vsample = (n_samps / AVX256Float::size) * AVX256Float::size;
 
 ibuffer_read_loop<AVX256Double, SSEFloat, fetch_float, Ip>(fetcher, out, offsets, fracts, n_vsample, mul);
 ibuffer_read_loop<Scalar<double>, Scalar<float>, fetch_float, Ip>(fetcher, out, offsets + n_vsample, fracts + n_vsample, n_samps - n_vsample, mul);
 }*/

template <class T, class Ft>
void table_read(Ft fetcher, T *out, intptr_t *offsets, T *fracts, intptr_t n_samps, T mul, InterpType interp)
{
    switch(interp)
    {
        case kInterpNone:           table_read<no_interp_reader>(fetcher, out, offsets, fracts, n_samps, mul);        break;
        case kInterpLinear:         table_read<linear_reader>(fetcher, out, offsets, fracts, n_samps, mul);           break;
        case kInterpCubicHermite:   table_read<cubic_hermite_reader>(fetcher, out, offsets, fracts, n_samps, mul);    break;
        case kInterpCubicLagrange:  table_read<cubic_lagrange_reader>(fetcher, out, offsets, fracts, n_samps, mul);   break;
        case kInterpCubicBSpline:   table_read<cubic_bspline_reader>(fetcher, out, offsets, fracts, n_samps, mul);    break;
    }
}

#endif /* TableReader_h */
