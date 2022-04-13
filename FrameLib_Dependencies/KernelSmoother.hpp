
#ifndef KERNELSMOOTHER_HPP
#define KERNELSMOOTHER_HPP

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>

#include "Allocator.hpp"
#include "SIMDSupport.hpp"
#include "SpectralProcessor.hpp"
#include "TableReader.hpp"

template <typename T, typename Allocator = aligned_allocator, bool auto_resize_fft = false>
class kernel_smoother : private spectral_processor<T, Allocator>
{
    using processor = spectral_processor<T, Allocator>;
    using op_sizes = typename processor::op_sizes;
    using zipped_pointer = typename processor::zipped_pointer;
    using in_ptr = typename processor::in_ptr;
 
    using Split = typename FFTTypes<T>::Split;
    
    template <bool B>
    using enable_if_t = typename std::enable_if<B, int>::type;
    
    enum class Ends { Zero, NonZero, SymZero, SymNonZero };
    
public:
    
    enum class EdgeMode { ZeroPad, Extend, Wrap, Fold, Mirror };
    
    template <typename U = Allocator, enable_if_t<std::is_default_constructible<U>::value> = 0>
    kernel_smoother(uintptr_t max_fft_size = 1 << 18)
    : spectral_processor<T, Allocator>(max_fft_size)
    {}
    
    template <typename U = Allocator, enable_if_t<std::is_copy_constructible<U>::value> = 0>
    kernel_smoother(const Allocator& allocator, uintptr_t max_fft_size = 1 << 18)
    : spectral_processor<T, Allocator>(allocator, max_fft_size)
    {}
    
    template <typename U = Allocator, enable_if_t<std::is_move_constructible<U>::value> = 0>
    kernel_smoother(const Allocator&& allocator, uintptr_t max_fft_size = 1 << 18)
    : spectral_processor<T, Allocator>(allocator, max_fft_size)
    {}

    void set_max_fft_size(uintptr_t size) { processor::set_max_fft_size(size); }
    
    uintptr_t max_fft_size() { return processor::max_fft_size(); }

    void smooth(T *out, const T *in, const T *kernel, uintptr_t length, uintptr_t kernel_length, double width_lo, double width_hi, bool symmetric, EdgeMode edges)
    {
        if (!length || !kernel_length)
            return;
        
        Allocator& allocator = processor::m_allocator;
        
        const int N = SIMDLimits<T>::max_size;
        
        width_lo = std::min(static_cast<double>(length), std::max(1.0, width_lo));
        width_hi = std::min(static_cast<double>(length), std::max(1.0, width_hi));
        
        double width_mul = (width_hi - width_lo) / (length - 1);
        
        auto half_width_calc = [&](uintptr_t a)
        {
            return static_cast<uintptr_t>(std::round((width_lo + a * width_mul) * 0.5));
        };
        
        uintptr_t filter_size = static_cast<uintptr_t>(std::ceil(std::max(width_lo, width_hi) * 0.5));
        uintptr_t filter_full = filter_size * 2 - 1;
        uintptr_t max_per_filter = static_cast<uintptr_t>(width_mul ? (2.0 / std::abs(width_mul)) + 1.0 : length);
        uintptr_t data_width = max_per_filter + (filter_full - 1);
        
        op_sizes sizes(data_width, filter_full, processor::EdgeMode::Linear);
        
        if (auto_resize_fft && processor::max_fft_size() < sizes.fft())
            set_max_fft_size(sizes.fft());
        
        uintptr_t fft_size = processor::max_fft_size() >= sizes.fft() ? sizes.fft() : 0;
        
        T *ptr = allocator.template allocate<T>(fft_size * 2 + filter_full + length + filter_size * 2);
        Split io { ptr, ptr + (fft_size >> 1) };
        Split st { io.realp + fft_size, io.imagp + fft_size };
        T *filter = ptr + (fft_size << 1);
        T *padded = filter + filter_full;
        
        Ends ends = Ends::NonZero;
        
        if (kernel_length)
        {
            const T max_value = *std::max_element(kernel, kernel + kernel_length);
            const T test_value_1 = kernel[0] / max_value;
            const T test_value_2 = kernel[kernel_length - 1] / max_value;
            const T epsilon = std::numeric_limits<T>::epsilon();
            
            if ((symmetric || test_value_1 < epsilon) && test_value_2 < epsilon)
                ends = symmetric ? Ends::SymZero : Ends::Zero;
        }
        
        // Copy data
        
        switch (edges)
        {
            case EdgeMode::ZeroPad:
                std::fill_n(padded, filter_size, 0.0);
                std::copy_n(in, length, padded + filter_size);
                std::fill_n(padded + filter_size + length, filter_size, 0.0);
                break;
                
            case EdgeMode::Extend:
                std::fill_n(padded, filter_size, in[0]);
                std::copy_n(in, length, padded + filter_size);
                std::fill_n(padded + filter_size + length, filter_size, in[length - 1]);
                break;
                
            case EdgeMode::Wrap:
                copy_edges<table_fetcher_wrap>(in, padded, length, filter_size);
            break;
                
            case EdgeMode::Fold:
                copy_edges<table_fetcher_fold>(in, padded, length, filter_size);
                break;
                
            case EdgeMode::Mirror:
                copy_edges<table_fetcher_mirror>(in, padded, length, filter_size);
                break;
        }
        
        if (symmetric)
        {
            // Offsets into the data and the filter
        
            const T *data = padded + filter_size;
            filter += filter_size - 1;
        
            // Symmetric filtering
            
            for (uintptr_t i = 0, j = 0; i < length; i = j)
            {
                const uintptr_t half_width = static_cast<uintptr_t>(half_width_calc(i));
                const uintptr_t width = half_width * 2 - 1;
                const T filter_half_sum = make_filter(filter, kernel, kernel_length, half_width, ends);
                const T filter_sum = (filter_half_sum * T(2) - filter[0]);
                const T gain = filter_sum ? T(1) / filter_sum : 1.0;

                for (j = i; (j < length) && half_width == half_width_calc(j); j++);
                
                uintptr_t n = j - i;
                uintptr_t m = use_fft_n(n, half_width, fft_size);
                uintptr_t k = 0;
                
                const double *data_fft = data - (half_width - 1);
                const double *filter_fft = filter - (half_width - 1);

                // Mirror the filter if required for the FFT processing

                if (m)
                {
                    for (intptr_t i = 1; i < static_cast<intptr_t>(half_width); i++)
                        filter[-i] = filter[i];
                }
                
                for (; k + (m - 1) < n; k += m)
                    apply_filter_fft(out + i + k, data_fft + i + k, filter_fft, io, st, width, m, gain);
                
                for (; k + (N - 1) < n; k += N)
                    apply_filter_symmetric<N>(out + i + k, data + i + k, filter, half_width, gain);
                
                for (; k < n; k++)
                    apply_filter_symmetric<1>(out + i + k, data + i + k, filter, half_width, gain);
            }
        }
        else
        {
            // Non-symmetric filtering

            for (uintptr_t i = 0, j = 0; i < length; i = j)
            {
                const uintptr_t half_width = static_cast<uintptr_t>(half_width_calc(i));
                const uintptr_t width = half_width * 2 - 1;
                const T filter_sum = make_filter(filter, kernel, kernel_length, width, ends);
                const T gain = filter_sum ? T(1) / filter_sum : 1.0;
                
                const T *data = padded + filter_size - (half_width - 1);

                for (j = i; (j < length) && half_width == half_width_calc(j); j++);
                
                uintptr_t n = j - i;
                uintptr_t m = use_fft_n(n, half_width, fft_size);
                uintptr_t k = 0;
                
                for (; k + (m - 1) < n; k += m)
                    apply_filter_fft(out + i + k, data + i + k, filter, io, st, width, m, gain);
                
                for (; k + (N - 1) < n; k += N)
                    apply_filter<N>(out + i + k, data + i + k, filter, width, gain);
                
                for (; k < n; k++)
                    apply_filter<1>(out + i + k, data + i + k, filter, width, gain);
            }
        }
        
        allocator.deallocate(ptr);
    }
    
private:
    
    struct fetcher : table_fetcher<double>
    {
        fetcher(const T *in, intptr_t size)
        : table_fetcher<T>(size, 1.0), data(in) {}
        
        T operator()(intptr_t idx) { return data[idx]; }
        
        const T *data;
    };

    template <template <class V> class U>
    void copy_edges(const T *in, T *out, intptr_t length, intptr_t filter_size)
    {
        intptr_t in_size = static_cast<intptr_t>(length);
        intptr_t edge_size = static_cast<intptr_t>(filter_size);
        
        U<fetcher> fetch(fetcher(in, in_size));
            
        for (intptr_t i = 0; i < edge_size; i++)
            out[i] = fetch(i - edge_size);
        
        std::copy_n(in, in_size, out + edge_size);
        
        for (intptr_t i = 0; i < edge_size; i++)
            out[i + in_size + edge_size] = fetch(i + in_size);
    }
    
    uintptr_t use_fft_n(uintptr_t n, uintptr_t half_width, uintptr_t fft_size)
    {
        bool use_fft = fft_size && n > 64 && half_width > 16 && (half_width * 64 > n);
        
        return use_fft ? n : 0;
    }

    T filter_kernel(const T *kernel, double position)
    {
        uintptr_t index = static_cast<uintptr_t>(position);
        
        const T lo = kernel[index];
        const T hi = kernel[index + 1];
        
        return static_cast<T>(lo + (position - index) * (hi - lo));
    }
    
    T make_filter(T *filter, const T *kernel, uintptr_t kernel_length, uintptr_t width, Ends ends)
    {
        if (kernel_length == 1)
        {
            std::fill_n(filter, width, kernel[0]);
            return filter[0] * width;
        }
        
        const double width_adjust = (ends == Ends::NonZero) ? -1.0 : (ends == Ends::SymZero ? 0.0 : 1.0);
        const double scale_width = std::max(1.0, width + width_adjust);
        const double width_normalise = static_cast<double>(kernel_length - 1) / scale_width;
        
        uintptr_t offset = ends == Ends::Zero ? 1 : 0;
        uintptr_t loop_size = ends == Ends::NonZero ? width - 1 : width;
        
        T filter_sum(0);
        
        for (uintptr_t j = 0; j < loop_size; j++)
        {
            filter[j] = filter_kernel(kernel, (j + offset) * width_normalise);
            filter_sum += filter[j];
        }
        
        if (ends == Ends::NonZero)
        {
            filter[width - 1] = kernel[kernel_length - 1];
            filter_sum += filter[width - 1];
        }
        
        return filter_sum;
    }
    
    template <int N>
    void apply_filter(T *out, const T *data, const T *filter, uintptr_t width, T gain)
    {
        using VecType = SIMDType<double, N>;
        
        VecType filter_val = filter[width - 1] * VecType(data);
        
        for (uintptr_t j = 1; j < width; j++)
            filter_val += filter[width - (j + 1)] * VecType(data + j);
        
        filter_val *= gain;
        filter_val.store(out);
    }
    
    template <int N>
    void apply_filter_symmetric(T *out, const T *data, const T *filter, uintptr_t half_width, T gain)
    {
        using VecType = SIMDType<double, N>;
        
        VecType filter_val = filter[0] * VecType(data);
        
        for (uintptr_t j = 1; j < half_width; j++)
            filter_val += filter[j] * (VecType(data - j) + VecType(data + j));
        
        filter_val *= gain;
        filter_val.store(out);
    }
    
    void apply_filter_fft(T *out, const T *data, const T *filter, Split& io, Split& temp, uintptr_t width, uintptr_t n, T gain)
    {
        uintptr_t data_width = n + width - 1;
        op_sizes sizes(data_width, width, processor::EdgeMode::Linear);
        in_ptr data_in(data, data_width);
        in_ptr filter_in(filter, width);
        
        // Process
        
        processor::template binary_op<ir_convolve_real>(io, temp, sizes, data_in, filter_in);
        
        // Copy output with scaling
        
        zipped_pointer p(io, width - 1);
        
        for (uintptr_t i = 0; i < n; i++)
            out[i] = *p++ * gain;
    }
};

#endif
