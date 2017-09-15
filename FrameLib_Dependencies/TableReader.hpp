
#ifndef TABLEREADER_H
#define TABLEREADER_H

#include "Interpolation.hpp"
#include "SIMDSupport.hpp"

// Generic types of interpolation

template <class T, class U, class Table> struct no_interp_reader
{
    no_interp_reader(Table fetcher) : fetch(fetcher) {}
    
    T operator()(double*& positions)
    {
        typename U::scalar_type array[T::size];
        
        for (int i = 0; i < T::size; i++)
            array[i] = fetch(static_cast<intptr_t>(*positions++));
        
        return U(array);
    }
    
    Table fetch;
};

template <class T, class U, class Table, typename Interp> struct interp_2_reader
{
    interp_2_reader(Table fetcher) : fetch(fetcher) {}
    
    T operator()(double*& positions)
    {
        typename T::scalar_type fract_array[T::size];
        typename U::scalar_type array[T::size * 2];
        
        for (int i = 0; i < T::size; i++)
        {
            double position = *positions++;
            intptr_t offset = static_cast<intptr_t>(position);
            fract_array[i] = static_cast<typename T::scalar_type >(position - static_cast<double>(offset));
            
            array[i] = fetch(offset);
            array[i + T::size] = fetch(offset + 1);
        }
        
        const T y0 = U(array);
        const T y1 = U(array + T::size);
        
        return interpolate(T(fract_array), y0, y1);
    }
    
    Table fetch;
    Interp interpolate;
};

template <class T, class U, class Table, typename Interp> struct interp_4_reader
{
    interp_4_reader(Table fetcher) : fetch(fetcher) {}
    
    T operator()(double*& positions)
    {
        typename U::scalar_type array[T::size * 4];
        typename T::scalar_type fract_array[T::size];

        for (int i = 0; i < T::size; i++)
        {
            double position = *positions++;
            intptr_t offset = static_cast<intptr_t>(position);
            fract_array[i] = static_cast<typename T::scalar_type >(position - static_cast<double>(offset));
            
            array[i] = fetch(offset - 1);
            array[i + T::size] = fetch(offset);
            array[i + T::size * 2] = fetch(offset + 1);
            array[i + T::size * 3] = fetch(offset + 2);
        }
        
        const T y0 = U(array);
        const T y1 = U(array + T::size);
        const T y2 = U(array + (T::size * 2));
        const T y3 = U(array + (T::size * 3));
        
        return interpolate(T(fract_array), y0, y1, y2, y3);
    }
    
    Table fetch;
    Interp interpolate;
};

template <class T, class U, class Table>
struct linear_reader : public interp_2_reader<T, U, Table, linear_interp<T> >
{
    linear_reader(Table fetcher) : interp_2_reader<T, U, Table, linear_interp<T> >(fetcher) {}
};

template <class T, class U, class Table>
struct cubic_bspline_reader : public interp_4_reader<T, U, Table, cubic_bspline_interp<T> >
{
    cubic_bspline_reader(Table fetcher) : interp_4_reader<T, U, Table, cubic_bspline_interp<T> >(fetcher) {}
};

template <class T, class U, class Table>
struct cubic_hermite_reader : public interp_4_reader<T, U, Table, cubic_hermite_interp<T> >
{
    cubic_hermite_reader(Table fetcher) : interp_4_reader<T, U, Table, cubic_hermite_interp<T> >(fetcher) {}
};

template <class T, class U, class Table>
struct cubic_lagrange_reader : public interp_4_reader<T, U, Table, cubic_lagrange_interp<T> >
{
    cubic_lagrange_reader(Table fetcher) : interp_4_reader<T, U, Table, cubic_lagrange_interp<T> >(fetcher) {}
};

template <class T, class U, class Table, template <class V, class W, class Tb> class Reader>
void table_read_loop(Table fetcher, typename T::scalar_type *out, double *positions, intptr_t n_samps, typename U::scalar_type mul)
{
    Reader<T, U, Table> reader(fetcher);
    
    T *v_out = reinterpret_cast<T *>(out);
    T scale = mul * reader.fetch.scale;
    
    for (intptr_t i = 0; i < (n_samps / T::size); i++)
        *v_out++ = scale * reader(positions);
}

template <template <class T, class U, class Tb> class Reader, class Table>
void table_read(Table fetcher, float *out, double *positions, intptr_t n_samps, float mul)
{
    intptr_t n_vsample = (n_samps / AVX256Float::size) * AVX256Float::size;
    
    // FIX - this will break for floats...
    
    table_read_loop<AVX256Float, AVX256Int32, Table, Reader>(fetcher, out, positions, n_vsample, mul);
    table_read_loop<Scalar<float>, Scalar<float>, Table, Reader>(fetcher, out, positions + n_vsample, n_samps - n_vsample, mul);
}

template <template <class T, class U, class Tb> class Reader, class Table>
void table_read(Table fetcher, double *out, double *positions, intptr_t n_samps, double mul)
{
    intptr_t n_vsample = (n_samps / AVX256Float::size) * AVX256Float::size;
    
    table_read_loop<AVX256Double, SSEFloat, Table, Reader>(fetcher, out, positions, n_vsample, mul);
    table_read_loop<Scalar<double>, Scalar<float>, Table, Reader>(fetcher, out, positions + n_vsample, n_samps - n_vsample, mul);
}

template <class T, class Table>
void table_read(Table fetcher, T *out, double *positions, intptr_t n_samps, T mul, InterpType interp)
{
    switch(interp)
    {
        case kInterpNone:           table_read<no_interp_reader>(fetcher, out, positions, n_samps, mul);        break;
        case kInterpLinear:         table_read<linear_reader>(fetcher, out, positions, n_samps, mul);           break;
        case kInterpCubicHermite:   table_read<cubic_hermite_reader>(fetcher, out,positions, n_samps, mul);    break;
        case kInterpCubicLagrange:  table_read<cubic_lagrange_reader>(fetcher, out, positions, n_samps, mul);   break;
        case kInterpCubicBSpline:   table_read<cubic_bspline_reader>(fetcher, out, positions, n_samps, mul);    break;
    }
}

#endif /* TableReader_h */
