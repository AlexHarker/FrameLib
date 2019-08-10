
#ifndef KERNELSMOOTHER_H
#define KERNELSMOOTHER_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>

#include "SIMDSupport.hpp"

enum SmoothMode { kSmoothZeroPad, kSmoothWrap, kSmoothFold };

template <typename T>
T filter_kernel(const T *kernel, double position)
{
    uintptr_t index = static_cast<uintptr_t>(position);
    
    const T lo = kernel[index];
    const T hi = kernel[index + 1];
    
    return static_cast<T>(lo + (position - index) * (hi - lo));
}

template <typename T>
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

template <int N, typename T>
void apply_filter(T *out, const T *data, const T *filter, uintptr_t half_width, T gain)
{
    using VecType = SIMDType<double, N>;
    
    VecType filter_val = SIMDType<double, N>(data) * filter[0];
    
    for (uintptr_t j = 1; j < half_width; j++)
        filter_val += filter[j] * (VecType(data -j) + VecType(data + j));
    
    filter_val *= gain;
    filter_val.store(out);
}

template <typename T, typename Allocator>
void kernel_smooth(T *out, const T *in, const T *kernel, uintptr_t length, uintptr_t kernel_length, double width_lo, double width_hi, SmoothMode mode, Allocator allocator)
{
    const int N = SIMDLimits<T>::max_size;

    width_lo = std::min(static_cast<double>(length), std::max(1.0, width_lo));
    width_hi = std::min(static_cast<double>(length), std::max(1.0, width_hi));
    
    double width_mul = (width_hi - width_lo) / (length - 1);
    
    auto half_width_calc = [&](uintptr_t a)
    {
        return static_cast<uintptr_t>(std::round((width_lo + a * width_mul) * 0.5));
    };
                                                                
    uintptr_t filter_size = std::ceil(std::max(width_lo, width_hi) * 0.5);
    
    T *filter = allocator.template alloc<T>(filter_size);
    T *temp = allocator.template alloc<T>(length + filter_size * 2);
    
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
    
    switch (mode)
    {
        case kSmoothZeroPad:
            std::fill_n(temp, filter_size, 0.0);
            std::copy_n(in, length, temp + filter_size);
            std::fill_n(temp + filter_size + length, filter_size, 0.0);
            break;
    
        case kSmoothWrap:
            std::copy_n(in + length - filter_size, filter_size, temp);
            std::copy_n(in, length, temp + filter_size);
            std::copy_n(in, filter_size, temp + filter_size + length);
            break;
            
        case kSmoothFold:
            std::reverse_copy(in + 1, in + 1 + filter_size, temp);
            std::copy(in, in + length, temp + filter_size);
            std::reverse_copy(in, in + filter_size, temp + filter_size + length);
            break;
    }
    
    const double *data = temp + filter_size;
    
    for (uintptr_t i = 0, j = 0; i < length; i = j)
    {
        uintptr_t half_width = static_cast<uintptr_t>(half_width_calc(i));
        const T filter_normalise = make_filter(filter, kernel, kernel_length, half_width, non_zero_end);
                
        for (j = i; (j < length) && half_width == half_width_calc(j); j++);
        
        uintptr_t n = j - i;
        uintptr_t k = 0;
        
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
    
    allocator.dealloc(filter);
    allocator.dealloc(temp);
}

struct malloc_allocator
{
    template <typename T>
    T* alloc(size_t size) { return reinterpret_cast<T*>(malloc(size * sizeof(T))); }
    
    template <typename T>
    void dealloc(T *ptr) { free(ptr); }
};

template <typename T>
void kernel_smooth(T *out, const T *in, const T *kernel, uintptr_t length, uintptr_t kernel_length, double width_lo, double width_hi, SmoothMode mode)
{
    kernel_smooth(out, in, kernel, length, kernel_length, width_lo, width_hi, mode, malloc_allocator());
}
#endif
