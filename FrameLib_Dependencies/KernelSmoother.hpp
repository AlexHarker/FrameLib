
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

template <typename T, typename Allocator = aligned_allocator, bool auto_resize_fft = false>
class kernel_smoother : private spectral_processor<T, Allocator>
{
    using processor = spectral_processor<T, Allocator>;
    using op_sizes = typename processor::op_sizes;
    using zipped_pointer = typename processor::zipped_pointer;
    using in_ptr = typename processor::in_ptr;
 
    using Split = typename FFTTypes<T>::Split;
    using EdgeMode = typename processor::EdgeMode;
    
    template <bool B>
    using enable_if_t = typename std::enable_if<B, int>::type;
    
public:
    
    enum EdgeType { kZeroPad, kExtend, kWrap, kFold, kMirror };
    
    template <typename U = Allocator, enable_if_t<std::is_default_constructible<U>::value> = 0>
    kernel_smoother()
    {
        set_max_fft_size(1 << 18);
    }
    
    template <typename U = Allocator, enable_if_t<std::is_copy_constructible<U>::value> = 0>
    kernel_smoother(const Allocator& allocator) : spectral_processor<T, Allocator>(allocator)
    {
        set_max_fft_size(1 << 18);
    }
    
    template <typename U = Allocator, enable_if_t<std::is_move_constructible<U>::value> = 0>
    kernel_smoother(Allocator&& allocator) : spectral_processor<T, Allocator>(allocator)
    {
        set_max_fft_size(1 << 18);
    }

    void set_max_fft_size(uintptr_t size) { processor::set_max_fft_size(size); }
    
    void smooth(T *out, const T *in, const T *kernel, uintptr_t length, uintptr_t kernel_length, double width_lo, double width_hi, EdgeType edges)
    {
        if (!length)
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
        uintptr_t max_per_filter = static_cast<uintptr_t>(width_mul ? (2.0 / width_mul) + 1.0 : length);
        uintptr_t data_width = max_per_filter + (filter_size - 1) * 2;
        
        op_sizes sizes(filter_full, data_width, EdgeMode::kEdgeLinear);
        
        if (auto_resize_fft && processor::max_fft_size() < sizes.fft())
            set_max_fft_size(sizes.fft());
        
        uintptr_t fft_size = processor::max_fft_size() >= sizes.fft() ? sizes.fft() : 0;
        
        T *ptr = allocator.template allocate<T>(fft_size * 2 + filter_full + length + filter_size * 2);
        Split io { ptr, ptr + (fft_size >> 1) };
        Split st { io.realp + fft_size, io.imagp + fft_size };
        T *filter = ptr + (fft_size << 1) + filter_size - 1;
        T *temp = filter + filter_size;
        
        bool non_zero_end = true;
        
        if (kernel_length)
        {
            const T max_value = *std::max_element(kernel, kernel + kernel_length);
            const T test_value = kernel[kernel_length - 1] / max_value;
            const T epsilon = std::numeric_limits<T>::epsilon();
            
            if (test_value < epsilon)
                non_zero_end = false;
        }
        
        // Copy data
        
        switch (edges)
        {
            case kZeroPad:
                std::fill_n(temp, filter_size, 0.0);
                std::copy_n(in, length, temp + filter_size);
                std::fill_n(temp + filter_size + length, filter_size, 0.0);
                break;
                
            case kExtend:
                std::fill_n(temp, filter_size, in[0]);
                std::copy_n(in, length, temp + filter_size);
                std::fill_n(temp + filter_size + length, filter_size, in[length - 1]);
                break;
                
            case kWrap:
                // FIX - cases where the input wraps more than once
                std::copy_n(in + length - filter_size, filter_size, temp);
                std::copy_n(in, length, temp + filter_size);
                std::copy_n(in, filter_size, temp + filter_size + length);
                break;
                
            case kFold:
                // FIX - cases where the input folds more than once
                std::reverse_copy(in + 1, in + 1 + filter_size, temp);
                std::copy(in, in + length, temp + filter_size);
                std::reverse_copy(in + length - (filter_size + 1), in + length - 1, temp + length + filter_size);
                break;
                
            case kMirror:
                // FIX - cases where the input mirros more than once
                std::reverse_copy(in, in + filter_size, temp);
                std::copy(in, in + length, temp + filter_size);
                std::reverse_copy(in + length - filter_size, in + length, temp + length + filter_size);
                break;
        }
        
        const double *data = temp + filter_size;
        
        for (uintptr_t i = 0, j = 0; i < length; i = j)
        {
            uintptr_t half_width = static_cast<uintptr_t>(half_width_calc(i));
            const T filter_normalise = make_filter(filter, kernel, kernel_length, half_width, non_zero_end);
            
            for (j = i; (j < length) && half_width == half_width_calc(j); j++);
            
            //uintptr_t optimal_fft = 1 << processor::calc_fft_size_log2(half_width * 4);
            uintptr_t n = j - i;
            uintptr_t k = 0;
            uintptr_t m = n;//std::min(optimal_fft / 2, n);
                
            m = use_fft(n, half_width, fft_size) ? m : 0;

            for (; k + (m - 1) < n; k += m)
                apply_filter_fft(out + i + k, data + i + k, filter, io, st, half_width, m, filter_normalise);
                
            for (; k + (N - 1) < n; k += N)
                apply_filter<N>(out + i + k, data + i + k, filter, half_width, filter_normalise);
            
            for (; k < n; k++)
                apply_filter<1>(out + i + k, data + i + k, filter, half_width, filter_normalise);
        }
        /*
         for (uintptr_t i = 0; i < length; i++)
         {
         // FIX - not safe
         double width = width_lo + i * width_mul;
         double width_normalise = 2.0 / width;
         uintptr_t half_width = width * 0.5;
         
         T filter_sum = kernel[0];
         T filter_val = data[i] * filter_sum;
         
         for (uintptr_t j = 1; j < half_width; j++)
         {
         T filter = filter_kernel(kernel, j * (kernel_length - 1) * width_normalise);
         filter_val += filter * (data[i - j] + data[i + j]);
         filter_sum += filter + filter;
         }
         
         out[i] = filter_val / filter_sum;
         }*/
        
        allocator.deallocate(ptr);
    }
    
private:
    
    bool use_fft(uintptr_t n, uintptr_t half_width, uintptr_t fft_size)
    {
        return fft_size && n > 2 && half_width > 2 && (32 * n > half_width);
    }
    
    T filter_kernel(const T *kernel, double position)
    {
        uintptr_t index = static_cast<uintptr_t>(position);
        
        const T lo = kernel[index];
        const T hi = kernel[index + 1];
        
        return static_cast<T>(lo + (position - index) * (hi - lo));
    }
    
    T make_filter(T *filter, const T *kernel, uintptr_t kernel_length, uintptr_t half_width, bool non_zero_end)
    {
        const double width_normalise = 1.0 / std::max(uintptr_t(1), half_width - (non_zero_end ? 1 : 0));
        T filter_sum = 0.0;
        
        uintptr_t loop_size = non_zero_end ? half_width - 1 : half_width;
        
        for (uintptr_t j = 0; j < loop_size; j++)
        {
            filter[j] = filter_kernel(kernel, j * (kernel_length - 1) * width_normalise);
            filter_sum += filter[j];
        }
        
        if (non_zero_end)
        {
            filter[half_width - 1] = kernel[kernel_length - 1];
            filter_sum += filter[half_width - 1];
        }
        
        return T(1) / (filter_sum * T(2) - filter[0]);
    }
    
    template <int N>
    void apply_filter(T *out, const T *data, const T *filter, uintptr_t half_width, T gain)
    {
        using VecType = SIMDType<double, N>;
        
        VecType filter_val = SIMDType<double, N>(data) * filter[0];
        
        for (uintptr_t j = 1; j < half_width; j++)
            filter_val += filter[j] * (VecType(data - j) + VecType(data + j));
        
        filter_val *= gain;
        filter_val.store(out);
    }
    
    void apply_filter_fft(T *out, const T *data, T *filter, Split& io, Split& temp, uintptr_t half_width, uintptr_t n, T gain)
    {
        uintptr_t filter_width = half_width * 2 - 1;
        uintptr_t data_width = n + (half_width - 1) * 2;
        op_sizes sizes(data_width, filter_width, EdgeMode::kEdgeLinear);
        in_ptr data_in(data - (half_width - 1), data_width);
        in_ptr filter_in(filter - (half_width - 1), filter_width);
        
        // Mirror the filter
        
        for (intptr_t i = 1; i < (intptr_t) half_width; i++)
            filter[-i] = filter[i];
        
        // Process
        
        processor::template binary_op<ir_convolve_real>(io, temp, sizes, data_in, filter_in);
        
        // Copy output with scaling
        
        zipped_pointer p(io, half_width - 1);
        
        for (uintptr_t i = 0; i < n; i++)
            out[i] = *p++ * gain;
    }
};
    
    
#endif
