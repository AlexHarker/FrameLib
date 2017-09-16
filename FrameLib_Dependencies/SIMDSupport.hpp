
/*
 *  AH_VectorOps
 *
 *	This header file provides a platform independent interface for SIMD operations (or vector operations).
 *	Code using this interface should compile for SSE or Altivec processors (and is also open to future changes in processor architecture).
 *	The list of operations here is by no means exhasutive, and is updated as needed using a consistent naming scheme.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifndef _AH_CROSS_PLATFORM_VECTOR_OPS_
#define _AH_CROSS_PLATFORM_VECTOR_OPS_ 

#include <emmintrin.h>
#include <immintrin.h>

#ifdef __APPLE__

#define FORCE_INLINE				__attribute__ ((always_inline))
#define FORCE_INLINE_DEFINITION

#else

#define FORCE_INLINE				__forceinline
#define FORCE_INLINE_DEFINITION		__forceinline;

#endif

/*
// Test for intel compilation

#ifndef TARGET_INTEL
#if defined( __i386__ ) || defined( __x86_64__ ) || defined(WIN_VERSION)
#define TARGET_INTEL
#endif
#endif

// Define for 64 bit float vector in 128bits (2 doubles)

#ifdef TARGET_INTEL
#ifndef VECTOR_F64_128BIT
#define VECTOR_F64_128BIT
#endif
#endif

// Runtime test for SSE2

static __inline int SSE2_check()
{
#ifdef __APPLE__
	return 1;
#else
	int SSE2_flag = 0;
	int CPUInfo[4] = {-1, 0, 0, 0};
	int nIds;

	__cpuid(CPUInfo, 0);
	 nIds = CPUInfo[0];

	if (nIds > 0)
	{
		__cpuid(CPUInfo, 1);
		SSE2_flag = (CPUInfo[3] >> 26) & 0x1;
	}
	
	return SSE2_flag;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// Macros for platform-specific vector ops ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TARGET_INTEL

// Load / Store / Unpack
 
#define F32_VEC_USTORE					_mm_storeu_ps
#define F32_VEC_MOVE_LO					_mm_movelh_ps
#define F32_VEC_MOVE_HI					_mm_movehl_ps
#define F64_VEC_USTORE					_mm_storeu_pd
#define F64_VEC_UNPACK_LO				_mm_unpacklo_pd
#define F64_VEC_UNPACK_HI				_mm_unpackhi_pd
#define F64_VEC_STORE_HI				_mm_storeh_pd
#define F64_VEC_STORE_LO				_mm_storel_pd

// Conversions

#define F32_VEC_FROM_I32				_mm_cvtepi32_ps
#define I32_VEC_FROM_F32_ROUND			_mm_cvtps_epi32
#define I32_VEC_FROM_F32_TRUNC			_mm_cvttps_epi32
 
#define F64_VEC_FROM_F32				_mm_cvtps_pd
#define F32_VEC_FROM_F64				_mm_cvtpd_ps

#define F64_VEC_FROM_I32				_mm_cvtepi32_pd 
#define I32_VEC_FROM_F64_ROUND			_mm_cvtpd_epi32
#define I32_VEC_FROM_F64_TRUNC			_mm_cvttpd_epi32

// Shuffles
 
#define F32_VEC_SHUFFLE					_mm_shuffle_ps
#define F64_VEC_SHUFFLE					_mm_shuffle_pd
#define I32_VEC_SHUFFLE_OP				_mm_shuffle_epi32
*/

#ifdef __APPLE__

#include <cpuid.h>

template <class T> T *allocate_aligned(size_t size)
{
    return static_cast<T *>(malloc(size * sizeof(T)));
}

template <class T> void deallocate_aligned(T *ptr)
{
    free(ptr);
}
/*
void cpuid(int32_t out[4], int32_t x)
{
    __cpuid_count(x, 0, out[0], out[1], out[2], out[3]);
}

uint64_t xgetbv(unsigned int index)
{
    uint32_t eax, edx;
    __asm__ __volatile__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(index));
    return ((uint64_t)edx << 32) | eax;
}
*/
#else
#include <malloc.h>

template <class T> T *allocate_aligned(size_t size)
{
    return static_cast<T *>(_aligned_malloc(size * sizeof(T), 16));
}

template <class T> void deallocate_aligned(T *ptr)
{
    _aligned_free(ptr);
}
/*
void cpuid(int32_t out[4], int32_t x)
{
    __cpuid(out, x);
}

uint64_t xgetbv(unsigned int x)
{
    return _xgetbv(x);
}
*/
#endif


#include <algorithm>
#include <functional>

#define SIMD_COMPILER_SUPPORT_SCALAR 0
#define SIMD_COMPILER_SUPPORT_SSE128 1
#define SIMD_COMPILER_SUPPORT_AVX256 2
#define SIMD_COMPILER_SUPPORT_AVX512 3

#if defined(__AVX512F__)
#define SIMD_COMPILER_SUPPORT_LEVEL SIMD_COMPILER_SUPPORT_AVX512
#elif defined(__AVX__)
#define SIMD_COMPILER_SUPPORT_LEVEL SIMD_COMPILER_SUPPORT_AVX256
#elif defined(__SSE__)
#define SIMD_COMPILER_SUPPORT_LEVEL SIMD_COMPILER_SUPPORT_SSE128
#else
#define SIMD_COMPILER_SUPPORT_LEVEL SIMD_COMPILER_SUPPORT_SCALAR
#endif

// Select Functionality for all types

template <class T> T select(const T& a, const T& b, const T& mask)
{
    return (b & mask) | and_not(mask, a);
}

// Data Type Definitions

// ******************** A Vector of Given Size (Made of Vectors / Scalars) ******************** //

template <int final_size, class T> struct SizedVector
{
    typedef SizedVector SV;
    typedef typename T::scalar_type scalar_type;
    static const int size = final_size;
    static const int array_size = final_size / T::size;
    
    SizedVector() {}
    SizedVector(const typename T::scalar_type& a)
    {
        for (int i = 0; i < array_size; i++)
            mData[i] = a;
    }
    SizedVector(const SizedVector *ptr) { *this = *ptr; }
    SizedVector(const typename T::scalar_type *array) { *this = *reinterpret_cast<const SizedVector *>(array); }
    
    // This template allows a static loop
    
    template <int First, int Last>
    struct static_for
    {
        template <typename Fn>
        void operator()(SizedVector &result, const SizedVector &a, const SizedVector &b, Fn const& fn) const
        {
            if (First < Last)
            {
                result.mData[First] = fn(a.mData[First], b.mData[First]);
                static_for<First + 1, Last>()(result, a, b, fn);
            }
        }
    };
    
    // This specialisation avoids infinite recursion
    
    template <int N>
    struct static_for<N, N>
    {
        template <typename Fn>
        void operator()(SV &result, const SV &a, const SV &b, Fn const& fn) const {}
    };
    
    template <typename Op> friend SizedVector op(const SV& a, const SV& b, Op op)
    {
        SV result;
        
        static_for<0, array_size>()(result, a, b, op);
        
        return result;
    }
    
    friend SV operator + (const SV& a, const SV& b) { return op(a, b, std::plus<T>()); }
    friend SV operator - (const SV& a, const SV& b) { return op(a, b, std::minus<T>()); }
    friend SV operator * (const SV& a, const SV& b) { return op(a, b, std::multiplies<T>()); }
    friend SV operator / (const SV& a, const SV& b) { return op(a, b, std::divides<T>()); }
    
    //friend SV sqrt(const SV& a) { return sqrt(a.mVal); }
    
    friend SV min(const SV& a, const SV& b) { return op(a, b, std::min<T>()); }
    friend SV max(const SV& a, const SV& b) { return op(a, b, std::max<T>()); }
    
    //friend SV and_not(const SV& a, const SV& b) { return (~a.mVal) &  b.mVal; }
    //friend SV operator & (const SV& a, const SV& b) { return a.mVal & b.mVal; }
    //friend SV operator | (const SV& a, const SV& b) { return a.mVal | b.mVal; }
    //friend SV operator ^ (const SV& a, const SV& b) { return a.mVal ^ b.mVal; }
    
    friend SV operator == (const SV& a, const SV& b) { return op(a, b, std::equal_to<T>()); }
    friend SV operator != (const SV& a, const SV& b) { return op(a, b, std::not_equal_to<T>()); }
    friend SV operator > (const SV& a, const SV& b) { return op(a, b, std::greater<T>());; }
    friend SV operator < (const SV& a, const SV& b) { return op(a, b, std::less<T>()); }
    friend SV operator >= (const SV& a, const SV& b) { return op(a, b, std::greater_equal<T>()); }
    friend SV operator <= (const SV& a, const SV& b) { return op(a, b, std::less_equal<T>()); }
    
    T mData[array_size];
};

// ******************** Basic Data Type Defintions ******************** //

template <class T> struct Scalar
{
    static const int size = 1;
    typedef T scalar_type;
    
    Scalar() {}
    Scalar(T a) : mVal(a) {}
    Scalar(const T* a) { mVal = *a; }
    template <class U> Scalar(const Scalar<U> a) { mVal = static_cast<T>(a.mVal); }
    
    friend Scalar operator + (const Scalar& a, const Scalar& b) { return a.mVal + b.mVal; }
    friend Scalar operator - (const Scalar& a, const Scalar& b) { return a.mVal - b.mVal; }
    friend Scalar operator * (const Scalar& a, const Scalar& b) { return a.mVal * b.mVal; }
    friend Scalar operator / (const Scalar& a, const Scalar& b) { return a.mVal / b.mVal; }

    friend Scalar sqrt(const Scalar& a) { return sqrt(a.mVal); }
  
    friend Scalar min(const Scalar& a, const Scalar& b) { return std::min(a.mVal, b.mVal); }
    friend Scalar max(const Scalar& a, const Scalar& b) { return std::max(a.mVal, b.mVal); }
    
    friend Scalar and_not(const Scalar& a, const Scalar& b) { return (~a.mVal) &  b.mVal; }
    friend Scalar operator & (const Scalar& a, const Scalar& b) { return a.mVal & b.mVal; }
    friend Scalar operator | (const Scalar& a, const Scalar& b) { return a.mVal | b.mVal; }
    friend Scalar operator ^ (const Scalar& a, const Scalar& b) { return a.mVal ^ b.mVal; }
    
    friend Scalar operator == (const Scalar& a, const Scalar& b) { return a.mVal == b.mVal; }
    friend Scalar operator != (const Scalar& a, const Scalar& b) { return a.mVal != b.mVal; }
    friend Scalar operator > (const Scalar& a, const Scalar& b) { return a.mVal > b.mVal; }
    friend Scalar operator < (const Scalar& a, const Scalar& b) { return a.mVal < b.mVal; }
    friend Scalar operator >= (const Scalar& a, const Scalar& b) { return a.mVal >= b.mVal; }
    friend Scalar operator <= (const Scalar& a, const Scalar& b) { return a.mVal <= b.mVal; }

    T mVal;
};

template <class T, class U, int vec_size> struct SIMDVector
{
    static const int size = vec_size;
    typedef T scalar_type;
    
    SIMDVector() {}
    SIMDVector(U a) : mVal(a) {}
    
    U mVal;
};

template <class T, int vec_size> struct SIMDType {};

#if (SIMD_COMPILER_SUPPORT_LEVEL >= SIMD_COMPILER_SUPPORT_SSE)

typedef SIMDType<double, 2> SSEDouble;
typedef SIMDType<float, 4> SSEFloat;
typedef SIMDType<int32_t, 4> SSEInt32;

template<>
struct SIMDType<double, 2> : public SIMDVector<double, __m128d, 2>
{
    SIMDType() {}
    SIMDType(const double& a) { mVal = _mm_set1_pd(a); }
    SIMDType(const double* a) { mVal = _mm_loadu_pd(a); }
    SIMDType(__m128d a) : SIMDVector(a) {}
    
    friend SIMDType operator + (const SIMDType &a, const SIMDType& b) { return _mm_add_pd(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType &a, const SIMDType& b) { return _mm_sub_pd(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType &a, const SIMDType& b) { return _mm_mul_pd(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType &a, const SIMDType& b) { return _mm_div_pd(a.mVal, b.mVal); }
    
    friend SIMDType sqrt(const SIMDType& a) { return _mm_sqrt_pd(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm_min_pd(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm_max_pd(a.mVal, b.mVal); }
    
    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm_andnot_pd(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm_and_pd(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm_or_pd(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm_xor_pd(a.mVal, b.mVal); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm_cmpeq_pd(a.mVal, b.mVal); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm_cmpneq_pd(a.mVal, b.mVal); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm_cmplt_pd(a.mVal, b.mVal); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm_cmpgt_pd(a.mVal, b.mVal); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm_cmple_pd(a.mVal, b.mVal); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm_cmpge_pd(a.mVal, b.mVal); }

    template <int y, int x> static SIMDType shuffle(const SIMDType& a, const SIMDType& b)
    {
        return _mm_shuffle_pd(a.mVal, b.mVal, (y<<1)|x);
    }
};

#if (SIMD_COMPILER_SUPPORT_LEVEL >= SIMD_COMPILER_SUPPORT_AVX256)

template <class T> struct AVX256Type {};

typedef SIMDType<double, 4> AVX256Double;
typedef SIMDType<float, 8> AVX256Float;
typedef SIMDType<int32_t, 8> AVX256Int32;

template<>
struct SIMDType<double, 4> : public SIMDVector<double, __m256d, 4>
{
    SIMDType() {}
    SIMDType(const double& a) { mVal = _mm256_set1_pd(a); }
    SIMDType(const double* a) { mVal = _mm256_loadu_pd(a); }
    SIMDType(__m256d a) : SIMDVector(a) {}
    
    friend SIMDType operator + (const SIMDType &a, const SIMDType &b) { return _mm256_add_pd(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType &a, const SIMDType &b) { return _mm256_sub_pd(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType &a, const SIMDType &b) { return _mm256_mul_pd(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType &a, const SIMDType &b) { return _mm256_div_pd(a.mVal, b.mVal); }
    
    friend SIMDType sqrt(const SIMDType& a) { return _mm256_sqrt_pd(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm256_min_pd(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm256_max_pd(a.mVal, b.mVal); }

    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm256_andnot_pd(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm256_and_pd(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm256_or_pd(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm256_xor_pd(a.mVal, b.mVal); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_EQ_OQ); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_NEQ_OQ); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_GT_OQ); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_LT_OQ); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_GE_OQ); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_LE_OQ); }
};

#endif

template<>
struct SIMDType<float, 4> : public SIMDVector<float, __m128, 4>
{
    SIMDType() {}
    SIMDType(const float& a) { mVal = _mm_set1_ps(a); }
    SIMDType(const float* a) { mVal = _mm_loadu_ps(a); }
    SIMDType(__m128 a) : SIMDVector(a) {}
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return _mm_add_ps(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return _mm_sub_ps(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return _mm_mul_ps(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType& a, const SIMDType& b) { return _mm_div_ps(a.mVal, b.mVal); }

    friend SIMDType sqrt(const SIMDType& a) { return _mm_sqrt_ps(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm_min_ps(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm_max_ps(a.mVal, b.mVal); }

    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm_andnot_ps(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm_and_ps(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm_or_ps(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm_xor_ps(a.mVal, b.mVal); }

    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm_cmpeq_ps(a.mVal, b.mVal); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm_cmpneq_ps(a.mVal, b.mVal); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm_cmplt_ps(a.mVal, b.mVal); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm_cmpgt_ps(a.mVal, b.mVal); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm_cmple_ps(a.mVal, b.mVal); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm_cmpge_ps(a.mVal, b.mVal); }

    template <int z, int y, int x, int w> static SIMDType shuffle(const SIMDType& a, const SIMDType& b)
    {
        return _mm_shuffle_ps(a.mVal, b.mVal, ((z<<6)|(y<<4)|(x<<2)|w));
    }
    
    operator AVX256Double() { return _mm256_cvtps_pd(mVal); }

    operator SizedVector<4, SSEDouble>()
    {
        SizedVector<4, SSEDouble> vec;
        
        vec.mData[0] = _mm_cvtps_pd(mVal);
        vec.mData[1] = _mm_cvtps_pd(_mm_movehl_ps(mVal, mVal));
        
        return vec;
    }
};

template<>
struct SIMDType<int32_t, 4> : public SIMDVector<int32_t, __m128i, 4>
{
    SIMDType() {}
    SIMDType(const int32_t& a) { mVal = _mm_set1_epi32(a); }
    SIMDType(const int32_t* a) { mVal = _mm_loadu_si128(reinterpret_cast<const __m128i *>(a)); }
    SIMDType(__m128 a) : SIMDVector(a) {}
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return _mm_add_epi32(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return _mm_sub_epi32(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return _mm_mul_epi32(a.mVal, b.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm_min_epi32(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm_max_epi32(a.mVal, b.mVal); }
    
    operator SSEFloat()     { return SSEFloat( _mm_cvtepi32_ps(mVal)); }
    operator AVX256Double() { return _mm256_cvtepi32_pd(mVal); }
    
    operator SizedVector<4, SSEDouble>()
    {
        SizedVector<4, SSEDouble> vec;
        
        vec.mData[0] = _mm_cvtepi32_pd(mVal);
        vec.mData[1] = _mm_cvtepi32_pd(_mm_shuffle_epi32(mVal, 0xE));
        
        return vec;
    }
};

#endif

#if (SIMD_COMPILER_SUPPORT_LEVEL >= SIMD_COMPILER_SUPPORT_AVX256)

template<>
struct SIMDType<float, 8> : public SIMDVector<float, __m256, 8>
{
    SIMDType() {}
    SIMDType(const float& a) { mVal = _mm256_set1_ps(a); }
    SIMDType(const float* a) { mVal = _mm256_loadu_ps(a); }
    SIMDType(__m256 a) : SIMDVector(a) {}
    
    friend SIMDType operator + (const SIMDType &a, const SIMDType &b) { return _mm256_add_ps(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType &a, const SIMDType &b) { return _mm256_sub_ps(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType &a, const SIMDType &b) { return _mm256_mul_ps(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType &a, const SIMDType &b) { return _mm256_div_ps(a.mVal, b.mVal); }
    
    friend SIMDType sqrt(const SIMDType& a) { return _mm256_sqrt_ps(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm256_min_ps(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm256_max_ps(a.mVal, b.mVal); }

    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm256_andnot_ps(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm256_and_ps(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm256_or_ps(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm256_xor_ps(a.mVal, b.mVal); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_EQ_OQ); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_NEQ_OQ); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_GT_OQ); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_LT_OQ); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_GE_OQ); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_LE_OQ); }
    
    operator SizedVector<8, AVX256Double>()
    {
        SizedVector<8, AVX256Double> vec;
        
        vec.mData[0] = _mm256_cvtps_pd(_mm256_extractf128_ps(mVal, 0));
        vec.mData[1] = _mm256_cvtps_pd(_mm256_extractf128_ps(mVal, 1));
        
        return vec;
    }
};

template<>
struct SIMDType<int32_t, 8> : public SIMDVector<int32_t, __m256i, 8>
{
    SIMDType() {}
    SIMDType(const int32_t& a) { mVal = _mm256_set1_epi32(a); }
    SIMDType(const int32_t* a) { mVal = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(a)); }
    SIMDType(__m256i a) : SIMDVector(a) {}
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return _mm256_add_epi32(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return _mm256_sub_epi32(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return _mm256_mul_epi32(a.mVal, b.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm256_min_epi32(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm256_max_epi32(a.mVal, b.mVal); }
 
    operator AVX256Float() { return AVX256Float( _mm256_cvtepi32_ps(mVal)); }
    
    operator SizedVector<8, AVX256Double>()
    {
        SizedVector<8, AVX256Double> vec;
        
        vec.mData[0] = _mm256_cvtepi32_pd(_mm256_extractf128_si256(mVal, 0));
        vec.mData[1] = _mm256_cvtepi32_pd(_mm256_extractf128_si256(mVal, 1));
        
        return vec;
    }
};

#endif

#if (SIMD_COMPILER_SUPPORT_LEVEL >= SIMD_COMPILER_SUPPORT_AVX512)

typedef SIMDType<double, 8> AVX512Double;
typedef SIMDType<float, 16> AVX512Float;

template<>
struct SIMDType<double, 8> : public SIMDVector<double, __m512d, 8>
{
    SIMDType() {}
    SIMDType(const double& a) { mVal = _mm512_set1_pd(a); }
    SIMDType(const double* a) { mVal = _mm512_loadu_pd(a); }
    SIMDType(__m512d a) : SIMDVector(a) {}
    
    friend SIMDType operator + (const SIMDType &a, const SIMDType &b) { return _mm512_add_pd(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType &a, const SIMDType &b) { return _mm512_sub_pd(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType &a, const SIMDType &b) { return _mm512_mul_pd(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType &a, const SIMDType &b) { return _mm512_div_pd(a.mVal, b.mVal); }
    
    friend SIMDType sqrt(const SIMDType& a) { return _mm512_sqrt_pd(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm512_min_pd(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm512_max_pd(a.mVal, b.mVal); }

    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm512_andnot_pd(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm512_and_pd(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm512_or_pd(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm512_xor_pd(a.mVal, b.mVal); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_EQ_OQ); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_NEQ_OQ); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_GT_OQ); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_LT_OQ); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_GE_OQ); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_LE_OQ); }
};

template<>
struct SIMDType<float, 16> : public SIMDVector<float, __m512, 16>
{
    SIMDType() {}
    SIMDType(const float& a) { mVal = _mm512_set1_ps(a); }
    SIMDType(const float* a) { mVal = _mm512_loadu_ps(a); }
    SIMDType(__m512 a) : SIMDVector(a) {}
    
    friend SIMDType operator + (const SIMDType &a, const SIMDType &b) { return _mm512_add_ps(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType &a, const SIMDType &b) { return _mm512_sub_ps(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType &a, const SIMDType &b) { return _mm512_mul_ps(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType &a, const SIMDType &b) { return _mm512_div_ps(a.mVal, b.mVal); }
    
    friend SIMDType sqrt(const SIMDType& a) { return _mm512_sqrt_ps(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm512_min_ps(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm512_max_ps(a.mVal, b.mVal); }
  
    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm512_andnot_ps(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm512_and_ps(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm512_or_ps(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm512_xor_ps(a.mVal, b.mVal); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_EQ_OQ); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_NEQ_OQ); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_GT_OQ); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_LT_OQ); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_GE_OQ); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_LE_OQ); }
};

#endif

#endif	/* _AH_CROSS_PLATFORM_VECTOR_OPS_ */
