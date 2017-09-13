
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

#ifdef __APPLE__

#include <Accelerate/Accelerate.h>

#define FORCE_INLINE				__attribute__ ((always_inline))
#define FORCE_INLINE_DEFINITION

#define ALIGNED_MALLOC malloc
#define ALIGNED_FREE free

#else

// Windows

#include <emmintrin.h>
#include <malloc.h>

#define FORCE_INLINE				__forceinline
#define FORCE_INLINE_DEFINITION		__forceinline;

#define ALIGNED_MALLOC(x)  _aligned_malloc(x, 16)
#define ALIGNED_FREE  _aligned_free

typedef	__m128i	vUInt8;
typedef __m128i vSInt8;
typedef	__m128i vUInt16;
typedef __m128i vSInt16;
typedef __m128i vUInt32;
typedef	__m128i vSInt32;
typedef	__m128i vBool32;
typedef __m128i vUInt64;
typedef __m128i vSInt64;
typedef	__m128  vFloat;
typedef	__m128d vDouble;

#endif

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
//////////////////////////////////////////////////// Utility macros (non platform-specific)  //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// The standard compare operations return all bits on, but for use in MSP we probably want values of one
// These routines can be used in this case

static const vFloat Vec_Ops_F32_One = {1.,1.,1.,1.};

#define F32_VEC_EQUAL_MSP_OP(a,b)		F32_VEC_AND_OP(Vec_Ops_F32_One, F32_VEC_EQUAL_OP(a,b)) 
#define F32_VEC_NOTEQUAL_MSP_OP(a,b)	F32_VEC_SUB_OP(Vec_Ops_F32_One, F32_VEC_AND_OP(F32_VEC_EQUAL_OP(a,b), Vec_Ops_F32_One))
#define F32_VEC_GT_MSP_OP(a,b)			F32_VEC_AND_OP(Vec_Ops_F32_One, F32_VEC_GT_OP(a,b))
#define F32_VEC_LT_MSP_OP(a,b)			F32_VEC_AND_OP(Vec_Ops_F32_One, F32_VEC_LT_OP(a,b)) 


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// Macros for platform-specific vector ops ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TARGET_INTEL

// Intel

// Comparisons that return one or zero

static const vDouble Vec_Ops_F64_One = {1.,1.};

#define F64_VEC_EQUAL_MSP_OP(a,b)		F64_VEC_AND_OP(Vec_Ops_F64_One, F64_VEC_EQUAL_OP(a,b)) 
#define F64_VEC_NOTEQUAL_MSP_OP(a,b)	F64_VEC_SUB_OP(Vec_Ops_F64_One, F64_VEC_AND_OP(F64_VEC_EQUAL_OP(a,b), Vec_Ops_F64_One))
#define F64_VEC_GT_MSP_OP(a,b)			F64_VEC_AND_OP(Vec_Ops_F64_One, F64_VEC_GT_OP(a,b))
#define F64_VEC_LT_MSP_OP(a,b)			F64_VEC_AND_OP(Vec_Ops_F64_One, F64_VEC_LT_OP(a,b)) 

// Floating point single precision (32 bit) intrinsics or local functions

#define float2vector					_mm_set1_ps

#define F32_VEC_MUL_OP					_mm_mul_ps
#define F32_VEC_DIV_OP					_mm_div_ps
#define F32_VEC_ADD_OP					_mm_add_ps
#define F32_VEC_SUB_OP					_mm_sub_ps

#define F32_VEC_AND_OP					_mm_and_ps
#define F32_VEC_ANDNOT_OP				_mm_andnot_ps
#define F32_VEC_OR_OP					_mm_or_ps
#define F32_VEC_XOR_OP					_mm_xor_ps

#define F32_VEC_SEL_OP					_mm_sel_ps

#define F32_VEC_MIN_OP					_mm_min_ps
#define F32_VEC_MAX_OP					_mm_max_ps

#define F32_VEC_EQUAL_OP(a,b)			_mm_cmpeq_ps(a,b)
#define F32_VEC_NEQUAL_OP(a,b)			_mm_cmpneq_ps(a,b)
#define F32_VEC_GT_OP(a,b)				_mm_cmpgt_ps(a,b)
#define F32_VEC_LT_OP(a,b)				_mm_cmplt_ps(a,b)

#define F32_VEC_SQRT_OP					_mm_sqrt_ps

#define F32_VEC_ULOAD					_mm_loadu_ps
#define F32_VEC_USTORE					_mm_storeu_ps
#define F32_VEC_MOVE_LO					_mm_movelh_ps
#define F32_VEC_MOVE_HI					_mm_movehl_ps
#define F32_VEC_SHUFFLE					_mm_shuffle_ps

// Conversions from and to 32 bit floating point vectors

#define F32_VEC_FROM_I32				_mm_cvtepi32_ps
#define I32_VEC_FROM_F32_ROUND			_mm_cvtps_epi32
#define I32_VEC_FROM_F32_TRUNC			_mm_cvttps_epi32

// Floating-point double precision (64 bit) intrinsics or local functions (only available under intel)

#define double2vector					_mm_set1_pd

#define F64_VEC_MUL_OP					_mm_mul_pd
#define F64_VEC_DIV_OP					_mm_div_pd
#define F64_VEC_ADD_OP					_mm_add_pd
#define F64_VEC_SUB_OP					_mm_sub_pd

#define F64_VEC_AND_OP					_mm_and_pd
#define F64_VEC_ANDNOT_OP				_mm_andnot_pd
#define F64_VEC_OR_OP					_mm_or_pd
#define F64_VEC_XOR_OP					_mm_xor_pd

#define F64_VEC_SEL_OP					_mm_sel_pd

#define F64_VEC_MIN_OP					_mm_min_pd
#define F64_VEC_MAX_OP					_mm_max_pd

#define F64_VEC_EQUAL_OP(a,b)			_mm_cmpeq_pd(a,b)
#define F64_VEC_NEQUAL_OP(a,b)			_mm_cmpneq_pd(a,b)
#define F64_VEC_GT_OP					_mm_cmpgt_pd
#define F64_VEC_LT_OP					_mm_cmplt_pd

#define F64_VEC_SQRT_OP					_mm_sqrt_pd

#define F64_VEC_ULOAD					_mm_loadu_pd
#define F64_VEC_USTORE					_mm_storeu_pd
#define F64_VEC_UNPACK_LO				_mm_unpacklo_pd
#define F64_VEC_UNPACK_HI				_mm_unpackhi_pd
#define F64_VEC_STORE_HI				_mm_storeh_pd
#define F64_VEC_STORE_LO				_mm_storel_pd
#define F64_VEC_SET_BOTH				_mm_set1_pd
#define F64_VEC_SHUFFLE					_mm_shuffle_pd

// Conversions from and to 64 bit floating point vectors

#define F64_VEC_FROM_F32				_mm_cvtps_pd
#define F32_VEC_FROM_F64				_mm_cvtpd_ps

#define F64_VEC_FROM_I32				_mm_cvtepi32_pd 
#define I32_VEC_FROM_F64_ROUND			_mm_cvtpd_epi32
#define I32_VEC_FROM_F64_TRUNC			_mm_cvttpd_epi32

// Integer 32 bit intrinsics

#define s32int2vector					_mm_set1_epi32

#define I32_VEC_ADD_OP					_mm_add_epi32
#define I32_VEC_SUB_OP					_mm_sub_epi32
#define I32_VEC_ADD_OP					_mm_add_epi32

#define I32_VEC_MIN_OP					_mm_min_epi32
#define I32_VEC_MAX_OP					_mm_max_epi32

#define I32_VEC_OR_OP					_mm_or_si128
#define I32_VEC_AND_OP					_mm_and_si128

#define I32_VEC_SHUFFLE_OP				_mm_shuffle_epi32

// Altivec has min / max intrinics for 32 bit signed integers, but on intel this must be done in software (although it is provided under windows)
// These routines are taken directly from the apple SSE migration guide
// The guide can be found at http://developer.apple.com/legacy/mac/library/documentation/Performance/Conceptual/Accelerate_sse_migration/Accelerate_sse_migration.pdf
/*
#ifdef __APPLE__
static __inline vSInt32 _mm_min_epi32(vSInt32 a, vSInt32 b) FORCE_INLINE;
static __inline vSInt32 _mm_min_epi32(vSInt32 a, vSInt32 b) 
{ 
	vSInt32 t = _mm_cmpgt_epi32(a,b);
	return _mm_or_si128( _mm_and_si128(t,b),_mm_andnot_si128(t,a));
}

static __inline vSInt32 _mm_max_epi32(vSInt32 a, vSInt32 b) FORCE_INLINE;
static __inline vSInt32 _mm_max_epi32(vSInt32 a, vSInt32 b) 
{ 
	vSInt32 t = _mm_cmpgt_epi32(a,b);
	return _mm_or_si128( _mm_andnot_si128(t,b),_mm_and_si128(t,a));
}
#endif 
*/
// Altivec has selection intrinics for 32 bit floating point vectors, but on intel this must be done in software
// These routines are taken directly from the apple SSE migration guide
// The guide can be found at http://developer.apple.com/legacy/mac/library/documentation/Performance/Conceptual/Accelerate_sse_migration/Accelerate_sse_migration.pdf

#ifdef __APPLE__
static __inline vFloat _mm_sel_ps(vFloat a, vFloat b, vFloat mask) FORCE_INLINE;
static __inline vUInt32 _mm_sel_epi32(vUInt32 a, vUInt32 b, vUInt32 mask) FORCE_INLINE;
#endif

static __inline vFloat _mm_sel_ps(vFloat a, vFloat b, vFloat mask) FORCE_INLINE_DEFINITION
{ 
    b = _mm_and_ps(b, mask); 
    a = _mm_andnot_ps(mask, a); 
    return _mm_or_ps(a, b); 
} 

static __inline vUInt32 _mm_sel_epi32(vUInt32 a, vUInt32 b, vUInt32 mask) FORCE_INLINE_DEFINITION
{ 
    b = _mm_and_si128(b, mask); 
    a = _mm_andnot_si128(mask, a); 
    return _mm_or_si128(a, b); 
} 

static __inline vDouble _mm_sel_pd(vDouble a, vDouble b, vDouble mask) FORCE_INLINE_DEFINITION
{ 
    b = _mm_and_pd(b, mask); 
    a = _mm_andnot_pd(mask, a); 
    return _mm_or_pd(a, b); 
} 

#else

// Altivec

static const vFloat Vec_Ops_F32_Zero = {0.f,0.f,0.f,0.f};

// Floating point 32 bit intrinsics or local functions

#define F32_VEC_MUL_OP(v1, v2)			vec_madd(v1,v2, Vec_Ops_F32_Zero)
#define F32_VEC_DIV_OP					vdivf
#define F32_VEC_ADD_OP					vec_add
#define F32_VEC_SUB_OP					vec_sub

#define F32_VEC_AND_OP					vec_and
#define F32_VEC_XOR_OP					vec_xor
#define F32_VEC_OR_OP					vec_or
#define F32_VEC_SEL_OP					vec_sel

#define F32_VEC_MIN_OP					vec_min
#define F32_VEC_MAX_OP					vec_max

#define F32_VEC_EQUAL_OP(a,b)			vec_cmpeq(a,b) 
#define F32_VEC_NEQUAL_OP(a,b)			vec_xor(vec_cmpeq(a,b),Vec_Ops_F32_One) 
#define F32_VEC_GT_OP(a,b)				vec_cmpgt(a,b) 
#define F32_VEC_LT_OP(a,b)				vec_cmplt(a,b)

#define F32_VEC_ULOAD(p)				vec_uload((unsigned char *)p)
#define F32_VEC_USTORE(p, v)			vec_ustore((unsigned char *)p, (vector unsigned char)v)
#define F32_VEC_SHUFFLE					vec_permute

// Conversions from and to 32 bit floating point vectors

#define F32_VEC_FROM_I32(a)				vec_ctf(a, 0)
#define I32_VEC_FROM_F32_ROUND(a)		vec_cts(vec_round(a), 0)
#define I32_VEC_FROM_F32_TRUNC(a)		vec_cts(a, 0)

// Integer 32 bit intrinsics

#define I32_VEC_ADD_OP					vec_add
#define I32_VEC_SUB_OP					vec_sub

#define I32_VEC_AND_OP					vec_and

#define I32_VEC_MIN_OP					vec_min
#define I32_VEC_MAX_OP					vec_max

// Return a vector filled with a single signed integer value

static __inline vSInt32 s32int2vector (int s32int_val) FORCE_INLINE;
static __inline vSInt32 s32int2vector (int s32int_val) FORCE_INLINE_DEFINITION
{
	vSInt32 TheVector = {s32int_val, s32int_val, s32int_val, s32int_val};
	return TheVector;
}

// Return a vector filled with a single float value

static __inline vFloat float2vector (float floatval) FORCE_INLINE;
static __inline vFloat float2vector (float floatval) FORCE_INLINE_DEFINITION
{
	vFloat TheVector;
	float *TheFloatArray = (float *) &TheVector;
	
	TheFloatArray[0] = floatval;
	TheFloatArray[1] = floatval;
	TheFloatArray[2] = floatval;
	TheFloatArray[3] = floatval;
	
	return TheVector;
}

// Provide altivec safe misaligned loads and stores (not sure how fast these are)
// These routines are taken directly from the apple SSE migration guide
// The guide can be found at http://developer.apple.com/legacy/mac/library/documentation/Performance/Conceptual/Accelerate_sse_migration/Accelerate_sse_migration.pdf


static inline vFloat vec_uload(unsigned char *target) FORCE_INLINE;
static inline vFloat vec_uload(unsigned char *target)										
{
    vector unsigned char MSQ, LSQ;
    vector unsigned char mask;

    MSQ = vec_ld(0, target);						// most significant quadword
    LSQ = vec_ld(15, target);						// least significant quadword
    mask = vec_lvsl(0, target);						// create the permute mask
    return (vFloat) vec_perm(MSQ, LSQ, mask);		// align the data
}


static __inline void vec_ustore(unsigned char *target, vector unsigned char src) FORCE_INLINE;
static __inline void vec_ustore(unsigned char *target, vector unsigned char src)				
{
    src = vec_perm( src, src, vec_lvsr( 0, target ) );
    vec_ste( (vector unsigned char) src, 0, (unsigned char*) target );
    vec_ste( (vector unsigned short) src,1,(unsigned short*) target );
    vec_ste( (vector unsigned int) src, 3, (unsigned int*) target );
    vec_ste( (vector unsigned int) src, 4, (unsigned int*) target );
    vec_ste( (vector unsigned int) src, 8, (unsigned int*) target );
    vec_ste( (vector unsigned int) src, 12, (unsigned int*) target );
    vec_ste( (vector unsigned short) src,14,(unsigned short*) target );
    vec_ste( (vector unsigned char) src,15,(unsigned char*) target );
}

#endif	/* TARGET_INTEL */

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

// Data Type Definitions

// ******************** A Vector of Given Size (Made of Vectors / Scalars) ******************** //

template <int final_size, class T> struct SizedVector
{
    static const int size = final_size;
    typedef typename T::scalar_type scalar_type;
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
        void operator()(SizedVector &result, const SizedVector &a, const SizedVector &b, Fn const& fn) const {}
    };
    
    template <typename Op> friend SizedVector operate(const SizedVector& a, const SizedVector& b, Op op)
    {
        SizedVector result;
        
        static_for<0, array_size>()(result, a, b, op);
        
        return result;
    }
    
    friend SizedVector operator + (const SizedVector& a, const SizedVector& b)
    {
        return operate(a, b, std::plus<T>());
    }
    
    friend SizedVector operator - (const SizedVector& a, const SizedVector& b)
    {
        return operate(a, b, std::minus<T>());
    }
    
    friend SizedVector operator * (const SizedVector& a, const SizedVector& b)
    {
        return operate(a, b, std::multiplies<T>());
    }
    
    T mData[array_size];
};

// ******************** Basic Data Type Defintions ******************** //

template <class T> struct Scalar
{
    static const int size = 1;
    typedef T scalar_type;
    
    Scalar() {}
    Scalar(T a) : mVal(a) {}
    friend Scalar operator + (const Scalar& a, const Scalar& b) { return Scalar(a.mVal + b.mVal); }
    friend Scalar operator - (const Scalar& a, const Scalar& b) { return Scalar(a.mVal - b.mVal); }
    friend Scalar operator * (const Scalar& a, const Scalar& b) { return Scalar(a.mVal * b.mVal); }
    
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

#if (SIMD_COMPILER_SUPPORT_LEVEL >= SIMD_COMPILER_SUPPORT_SSE)

struct SSEDouble : public SIMDVector<double, __m128d, 2>
{
    SSEDouble() {}
    SSEDouble(const double& a) { mVal = _mm_set1_pd(a); }
    SSEDouble(__m128d a) : SIMDVector(a) {}
    friend SSEDouble operator + (const SSEDouble &a, const SSEDouble& b) { return _mm_add_pd(a.mVal, b.mVal); }
    friend SSEDouble operator - (const SSEDouble &a, const SSEDouble& b) { return _mm_sub_pd(a.mVal, b.mVal); }
    friend SSEDouble operator * (const SSEDouble &a, const SSEDouble& b) { return _mm_mul_pd(a.mVal, b.mVal); }
    
    template <int y, int x> static SSEDouble shuffle(const SSEDouble& a, const SSEDouble& b)
    {
        return _mm_shuffle_pd(a.mVal, b.mVal, (y<<1)|x);
    }
};

struct SSEFloat : public SIMDVector<float, __m128, 4>
{
    SSEFloat() {}
    SSEFloat(const float& a) { mVal = _mm_set1_ps(a); }
    SSEFloat(__m128 a) : SIMDVector(a) {}
    friend SSEFloat operator + (const SSEFloat& a, const SSEFloat& b) { return _mm_add_ps(a.mVal, b.mVal); }
    friend SSEFloat operator - (const SSEFloat& a, const SSEFloat& b) { return _mm_sub_ps(a.mVal, b.mVal); }
    friend SSEFloat operator * (const SSEFloat& a, const SSEFloat& b) { return _mm_mul_ps(a.mVal, b.mVal); }
    
    template <int z, int y, int x, int w> static SSEFloat shuffle(const SSEFloat& a, const SSEFloat& b)
    {
        return _mm_shuffle_ps(a.mVal, b.mVal, ((z<<6)|(y<<4)|(x<<2)|w));
    }
    
    operator SizedVector<4, SSEDouble>()
    {
        SizedVector<4, SSEDouble> vec;
        
        vec.mData[0] = _mm_cvtps_pd(mVal);
        vec.mData[1] = _mm_cvtps_pd(_mm_movehl_ps(mVal, mVal));
        
        return vec;
    }
};

struct SSEInt32 : public SIMDVector<float, __m128i, 4>
{
    SSEInt32() {}
    SSEInt32(const int32_t& a) { mVal = _mm_set1_epi32(a); }
    SSEInt32(__m128 a) : SIMDVector(a) {}
    friend SSEInt32 operator + (const SSEInt32& a, const SSEInt32& b) { return _mm_add_epi32(a.mVal, b.mVal); }
    friend SSEInt32 operator - (const SSEInt32& a, const SSEInt32& b) { return _mm_sub_epi32(a.mVal, b.mVal); }
    friend SSEInt32 operator * (const SSEInt32& a, const SSEInt32& b) { return _mm_mul_epi32(a.mVal, b.mVal); }
    
    operator SSEFloat() { return SSEFloat( _mm_cvtepi32_ps(mVal)); }
    
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

struct AVX256Double : public SIMDVector<double, __m256d, 4>
{
    AVX256Double() {}
    AVX256Double(const double& a) { mVal = _mm256_set1_pd(a); }
    AVX256Double(__m256d a) : SIMDVector(a) {}
    friend AVX256Double operator + (const AVX256Double &a, const AVX256Double &b) { return _mm256_add_pd(a.mVal, b.mVal); }
    friend AVX256Double operator - (const AVX256Double &a, const AVX256Double &b) { return _mm256_sub_pd(a.mVal, b.mVal); }
    friend AVX256Double operator * (const AVX256Double &a, const AVX256Double &b) { return _mm256_mul_pd(a.mVal, b.mVal); }
};

struct AVX256Float : public SIMDVector<float, __m256, 8>
{
    AVX256Float() {}
    AVX256Float(const float& a) {  mVal = _mm256_set1_ps(a); }
    AVX256Float(__m256 a) : SIMDVector(a) {}
    friend AVX256Float operator + (const AVX256Float &a, const AVX256Float &b) { return _mm256_add_ps(a.mVal, b.mVal); }
    friend AVX256Float operator - (const AVX256Float &a, const AVX256Float &b) { return _mm256_sub_ps(a.mVal, b.mVal); }
    friend AVX256Float operator * (const AVX256Float &a, const AVX256Float &b) { return _mm256_mul_ps(a.mVal, b.mVal); }
    
    operator SizedVector<8, AVX256Double>()
    {
        SizedVector<8, AVX256Double> vec;
        
        vec.mData[0] = _mm256_cvtps_pd(_mm256_extractf128_ps(mVal, 0));
        vec.mData[1] = _mm256_cvtps_pd(_mm256_extractf128_ps(mVal, 1));
        
        return vec;
    }
};

struct AVX256Int32 : public SIMDVector<float, __m256i, 8>
{
    AVX256Int32() {}
    AVX256Int32(const int32_t& a) { mVal = _mm256_set1_epi32(a); }
    AVX256Int32(__m256i a) : SIMDVector(a) {}
    friend AVX256Int32 operator + (const AVX256Int32& a, const AVX256Int32& b) { return _mm256_add_epi32(a.mVal, b.mVal); }
    friend AVX256Int32 operator - (const AVX256Int32& a, const AVX256Int32& b) { return _mm256_sub_epi32(a.mVal, b.mVal); }
    friend AVX256Int32 operator * (const AVX256Int32& a, const AVX256Int32& b) { return _mm256_mul_epi32(a.mVal, b.mVal); }
    
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

struct AVX512Double : public SIMDVector<double, __m512d, 8>
{
    AVX512Double() {}
    AVX512Double(const double& a) { mVal = _mm512_set1_pd(a); }
    AVX512Double(__m512d a) : SIMDVector(a) {}
    friend AVX512Double operator + (const AVX512Double &a, const AVX512Double &b) { return _mm512_add_pd(a.mVal, b.mVal); }
    friend AVX512Double operator - (const AVX512Double &a, const AVX512Double &b) { return _mm512_sub_pd(a.mVal, b.mVal); }
    friend AVX512Double operator * (const AVX512Double &a, const AVX512Double &b) { return _mm512_mul_pd(a.mVal, b.mVal); }
};

struct AVX512Float : public SIMDVector<float, __m512, 16>
{
    AVX512Float() {}
    AVX512Float(const float& a) { mVal = _mm512_set1_ps(a); }
    AVX512Float(__m512 a) : SIMDVector(a) {}
    friend AVX512Float operator + (const AVX512Float &a, const AVX512Float &b) { return _mm512_add_ps(a.mVal, b.mVal); }
    friend AVX512Float operator - (const AVX512Float &a, const AVX512Float &b) { return _mm512_sub_ps(a.mVal, b.mVal); }
    friend AVX512Float operator * (const AVX512Float &a, const AVX512Float &b) { return _mm512_mul_ps(a.mVal, b.mVal); }
};

#endif

#endif	/* _AH_CROSS_PLATFORM_VECTOR_OPS_ */
