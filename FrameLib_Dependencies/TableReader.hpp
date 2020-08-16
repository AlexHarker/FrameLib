
#ifndef TABLEREADER_HPP
#define TABLEREADER_HPP

#include "SIMDSupport.hpp"
#include "Interpolation.hpp"
#include <algorithm>

// Enumeration of edge types

enum EdgeType
{
    kExtrapolate, kExtend, kWrap, kZeroPad
};

// Base class for table fetchers

// Implementations
// - must provide - T operator()(intptr_t offset) - which does the fetching of values
// - may also provide - template <class U, V> void split(U position, intptr_t& offset, V& fract, intptr_t N)
// - split() generates the offset and fractional interpolation values and may additionally constrain them
// - may also provide intptr_t limit() which should return the highest valid position for bounds etc.

template <class T>
struct table_fetcher
{
    typedef T fetch_type;
    
    table_fetcher(intptr_t length, double scale_val) : size(length), scale(scale_val) {}
    
    template <class U, class V>
    void split(U position, intptr_t& offset, V& fract, intptr_t N)
    {
        offset = static_cast<intptr_t>(std::floor(position));
        fract = static_cast<V>(position - static_cast<V>(offset));
    }
    
    intptr_t limit() { return size - 1; }
    
    const intptr_t size;
    const double scale;
};

// Adaptor to take a fetcher and extrapolate the input

template <class T>
struct table_fetcher_extrapolate : T
{
    table_fetcher_extrapolate(const T& base) : T(base){}
    
    template <class U, class V>
    void split(U position, intptr_t& offset, V& fract, intptr_t N)
    {
        U constrained = std::max(std::min(position, U(T::size - ((N >> 1) + 1))), U((N - 1) >> 1));
        offset = static_cast<intptr_t>(std::floor(constrained));
        fract = static_cast<V>(position - static_cast<V>(offset));
    }
};

// Adaptor to take a fetcher and make it extend the end values

template <class T>
struct table_fetcher_extend : T
{
    table_fetcher_extend(const T& base) : T(base){}
    
    typename T::fetch_type operator()(intptr_t offset)
    {
        return T::operator()(std::min(std::max(offset, static_cast<intptr_t>(0)), T::size - 1));
    }
};

// An adaptor to take a fetcher and make it pad with zeros at either end

template <class T>
struct table_fetcher_zeropad : T
{
    table_fetcher_zeropad(const T& base) : T(base){}
    
    typename T::fetch_type operator()(intptr_t offset)
    {
        return (offset < 0 || offset >= T::size) ? typename T::fetch_type(0) : T::operator()(offset);
    }
};

// Adaptor to take a fetcher and make it wrap

template <class T>
struct table_fetcher_wrap : T
{
    table_fetcher_wrap(const T& base) : T(base){}
    
    typename T::fetch_type operator()(intptr_t offset)
    {
        return T::operator()(offset < 0 ? T::size + (offset % T::size) : offset % T::size);
    }
    
    intptr_t limit() { return T::size; }
};

// Adaptor to take a fetcher and bound the input (can be added to the above edge behaviours)

template <class T>
struct table_fetcher_bound : T
{
    table_fetcher_bound(const T& base) : T(base){}

    template <class U, class V>
    void split(U position, intptr_t& offset, V& fract, intptr_t N)
    {
        position = std::max(std::min(position, U(T::limit())), U(0));
        T::split(position, offset, fract, N);
    }
};

// Generic interpolation readers

template <class T, class U, class V, class Table, typename Interp>
struct interp_2_reader
{
    interp_2_reader(Table fetcher) : fetch(fetcher) {}
    
    T operator()(const V*& positions)
    {
        typename T::scalar_type fract_array[T::size];
        typename U::scalar_type array[T::size * 2];
        
        for (int i = 0; i < T::size; i++)
        {
            intptr_t offset;

            fetch.split(*positions++, offset, fract_array[i], 2);
            
            array[i]            = fetch(offset + 0);
            array[i + T::size]  = fetch(offset + 1);
        }
        
        const T y0 = U(array);
        const T y1 = U(array + T::size);
        
        return interpolate(T(fract_array), y0, y1);
    }
    
    Table fetch;
    Interp interpolate;
};

template <class T, class U, class V, class Table, typename Interp>
struct interp_4_reader
{
    interp_4_reader(Table fetcher) : fetch(fetcher) {}
    
    T operator()(const V*& positions)
    {        
        typename T::scalar_type fract_array[T::size];
        typename U::scalar_type array[T::size * 4];
        
        for (int i = 0; i < T::size; i++)
        {
            intptr_t offset;
            
            fetch.split(*positions++, offset, fract_array[i], 4);
            
            array[i]                = fetch(offset - 1);
            array[i + T::size]      = fetch(offset + 0);
            array[i + T::size * 2]  = fetch(offset + 1);
            array[i + T::size * 3]  = fetch(offset + 2);
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

// Readers with specific interpolation types

template <class T, class U, class V, class Table>
struct no_interp_reader
{
    no_interp_reader(Table fetcher) : fetch(fetcher) {}
    
    T operator()(const V*& positions)
    {
        typename U::scalar_type array[T::size];
        
        for (int i = 0; i < T::size; i++)
        {
            typename T::scalar_type fract;
            intptr_t offset;

            fetch.split(*positions++, offset, fract, 0);
            array[i] = fetch(offset);
        }
        
        return U(array);
    }
    
    Table fetch;
};

template <class T, class U, class V, class Table>
struct linear_reader : public interp_2_reader<T, U, V, Table, linear_interp<T>>
{
    linear_reader(Table fetcher) : interp_2_reader<T, U, V, Table, linear_interp<T>>(fetcher) {}
};

template <class T, class U, class V,  class Table>
struct cubic_bspline_reader : public interp_4_reader<T, U, V, Table, cubic_bspline_interp<T>>
{
    cubic_bspline_reader(Table fetcher) : interp_4_reader<T, U, V, Table, cubic_bspline_interp<T>>(fetcher) {}
};

template <class T, class U, class V,  class Table>
struct cubic_hermite_reader : public interp_4_reader<T, U, V, Table, cubic_hermite_interp<T>>
{
    cubic_hermite_reader(Table fetcher) : interp_4_reader<T, U, V, Table, cubic_hermite_interp<T>>(fetcher) {}
};

template <class T, class U, class V, class Table>
struct cubic_lagrange_reader : public interp_4_reader<T, U, V, Table, cubic_lagrange_interp<T>>
{
    cubic_lagrange_reader(Table fetcher) : interp_4_reader<T, U, V, Table, cubic_lagrange_interp<T>>(fetcher) {}
};

// Reading loop

template <class T, class U, class V, class Table, template <class W, class X, class Y, class Tb> class Reader>
void table_read_loop(Table fetcher, typename T::scalar_type *out, const V *positions, intptr_t n_samps, double mul)
{
    Reader<T, U, V, Table> reader(fetcher);
    
    T *v_out = reinterpret_cast<T *>(out);
    T scale = static_cast<typename U::scalar_type>(mul * reader.fetch.scale);
    
    for (intptr_t i = 0; i < (n_samps / T::size); i++)
        *v_out++ = scale * reader(positions);
}

// Template to determine vector/scalar types

template <template <class T, class U, class V, class Tb> class Reader, class Table, class W, class X>
void table_read(Table fetcher, W *out, const X *positions, intptr_t n_samps, double mul)
{
    typedef typename Table::fetch_type fetch_type;
    const int vec_size = SIMDLimits<W>::max_size;
    intptr_t n_vsample = (n_samps / vec_size) * vec_size;
    
    table_read_loop<SIMDType<W, vec_size>, SIMDType<fetch_type, vec_size>, X, Table, Reader>(fetcher, out, positions, n_vsample, mul);
    table_read_loop<SIMDType<W, 1>, SIMDType<fetch_type, 1>, X, Table, Reader>(fetcher, out + n_vsample, positions + n_vsample, n_samps - n_vsample, mul);
}

// Main reading call that switches between different types of interpolation

template <class T, class U, class Table>
void table_read(Table fetcher, T *out, const U *positions, intptr_t n_samps, T mul, InterpType interp)
{
    switch(interp)
    {
        case kInterpNone:           table_read<no_interp_reader>(fetcher, out, positions, n_samps, mul);        break;
        case kInterpLinear:         table_read<linear_reader>(fetcher, out, positions, n_samps, mul);           break;
        case kInterpCubicHermite:   table_read<cubic_hermite_reader>(fetcher, out,positions, n_samps, mul);     break;
        case kInterpCubicLagrange:  table_read<cubic_lagrange_reader>(fetcher, out, positions, n_samps, mul);   break;
        case kInterpCubicBSpline:   table_read<cubic_bspline_reader>(fetcher, out, positions, n_samps, mul);    break;
    }
}

// Reading calls to add adaptors to the basic fetcher

template <class T, class U, class Table>
void table_read_optional_bound(Table fetcher, T *out, const U *positions, intptr_t n_samps, T mul, InterpType interp, bool bound)
{
    if (bound)
    {
        table_fetcher_bound<Table> fetch(fetcher);
        table_read(fetch, out, positions, n_samps, mul, interp);
    }
    else
        table_read(fetcher, out, positions, n_samps, mul, interp);
}

template <class T, class U, class Table>
void table_read_extrapolate(Table fetcher, T *out, const U *positions, intptr_t n_samps, T mul, InterpType interp, bool bound)
{
    table_fetcher_extrapolate<Table> fetch(fetcher);
    table_read_optional_bound(fetch, out, positions, n_samps, mul, interp, bound);
}

template <class T, class U, class Table>
void table_read_extend(Table fetcher, T *out, const U *positions, intptr_t n_samps, T mul, InterpType interp, bool bound)
{
    table_fetcher_extend<Table> fetch(fetcher);
    table_read_optional_bound(fetch, out, positions, n_samps, mul, interp, bound);
}

template <class T, class U, class Table>
void table_read_zeropad(Table fetcher, T *out, const U *positions, intptr_t n_samps, T mul, InterpType interp, bool bound)
{
    table_fetcher_zeropad<Table> fetch(fetcher);
    table_read_optional_bound(fetch, out, positions, n_samps, mul, interp, bound);
}

template <class T, class U, class Table>
void table_read_wrap(Table fetcher, T *out, const U *positions, intptr_t n_samps, T mul, InterpType interp, bool bound)
{
    table_fetcher_wrap<Table> fetch(fetcher);
    table_read_optional_bound(fetch, out, positions, n_samps, mul, interp, bound);
}

// Main read call for variable edge behaviour

template <class T, class U, class Table>
void table_read_edges(Table fetcher, T *out, const U *positions, intptr_t n_samps, T mul, InterpType interp, EdgeType edges, bool bound)
{
    switch (edges)
    {
        case kExtrapolate:  table_read_extrapolate(fetcher, out, positions, n_samps, mul, interp, bound);   break;
        case kExtend:       table_read_extend(fetcher, out, positions, n_samps, mul, interp, bound);        break;
        case kWrap:         table_read_wrap(fetcher, out, positions, n_samps, mul, interp, bound);          break;
        case kZeroPad:      table_read_zeropad(fetcher, out, positions, n_samps, mul, interp, bound);       break;
    }
}

#endif /* TableReader_h */
