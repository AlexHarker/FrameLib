
#ifndef FrameLib_TypeList_h
#define FrameLib_TypeList_h

#include "FrameLib_Objects.h"
#include <utility>

namespace detail
{
  template<typename...Ts> struct FrameLib_Typelist
  {
      template<template<typename> class F, typename...Args>
      static void execute(Args...args)
      {
        (void)std::initializer_list<int>{(F<Ts>{}(std::forward<Args>(args)...),0)...};
      }
  };
}

using FrameLib_ObjectList = detail::FrameLib_Typelist<

// Filters

FrameLib_0dfSVF,
FrameLib_OnePole,
FrameLib_OnePoleZero,
FrameLib_Resonant,
FrameLib_SallenAndKey,

// Generators

FrameLib_Gaussian,
FrameLib_Ramp,
FrameLib_Random,
FrameLib_Uniform,

// IO

FrameLib_Sink,
FrameLib_Source,
FrameLib_Trace,

// Mapping

FrameLib_Constant,
FrameLib_Convert,
FrameLib_Lookup,
FrameLib_Map,
FrameLib_SampleRate,

// Parameters

FrameLib_CombineTags,
FrameLib_FilterTags,
FrameLib_GetParam,
FrameLib_Tag,
FrameLib_Untag,

// Routing

FrameLib_Dispatch,
FrameLib_Route,
FrameLib_Select,

// Schedulers

FrameLib_AudioTrigger,
FrameLib_Future,
FrameLib_Interval,
FrameLib_Once,
FrameLib_PerBlock,

// Spatial

FrameLib_CoordinateSystem,
FrameLib_Spatial,

// Spectral

FrameLib_FFT,
FrameLib_iFFT,
FrameLib_Correlate,
FrameLib_Convolve,
FrameLib_Multitaper,
FrameLib_Window,

// Storage

FrameLib_Recall,
FrameLib_Register,
FrameLib_Store,

// Streaming

FrameLib_StreamID,

// Time Smoothing

FrameLib_EWMA,
FrameLib_EWMSD,
FrameLib_FrameDelta,
FrameLib_Lag,
FrameLib_TimeMean,
FrameLib_TimeMedian,
FrameLib_TimeStdDev,

// Timing

FrameLib_Now,
FrameLib_Ticks,
FrameLib_TimeDelta,
FrameLib_Timer,

// Vector

FrameLib_AccumPoint,
FrameLib_Chop,
FrameLib_Join,
FrameLib_MedianFilter,
FrameLib_NonZero,
FrameLib_Pad,
FrameLib_Peaks,
FrameLib_Percentile,
FrameLib_Reverse,
FrameLib_Shift,
FrameLib_Sort,
FrameLib_Split,
FrameLib_Subframe,

FrameLib_Length,
FrameLib_VectorMin,
FrameLib_VectorMax,
FrameLib_Sum,
FrameLib_Product,
FrameLib_Mean,
FrameLib_GeometricMean,
FrameLib_StandardDeviation,

FrameLib_Centroid,
FrameLib_Spread,
FrameLib_Skewness,
FrameLib_Kurtosis,
FrameLib_Flatness,
FrameLib_RMS,
FrameLib_Crest,
FrameLib_VectorArgMin,
FrameLib_VectorArgMax,
FrameLib_NanFilter,

// Unary Operators

FrameLib_LogicalNot,

FrameLib_Acos,
FrameLib_Asin,
FrameLib_Atan,
FrameLib_Cos,
FrameLib_Sin,
FrameLib_Tan,
FrameLib_Acosh,
FrameLib_Asinh,
FrameLib_Atanh,
FrameLib_Cosh,
FrameLib_Sinh,
FrameLib_Tanh,

FrameLib_Log,
FrameLib_Log2,
FrameLib_Log10,
FrameLib_Exp,
FrameLib_Exp2,

FrameLib_Abs,
FrameLib_Ceil,
FrameLib_Floor,
FrameLib_Round,
FrameLib_Trunc,

FrameLib_Sqrt,
FrameLib_Cbrt,
FrameLib_Erf,
FrameLib_Erfc,

// Binary  Operators

FrameLib_Plus,
FrameLib_Minus,
FrameLib_Multiply,
FrameLib_Divide,

FrameLib_Equal,
FrameLib_NotEqual,
FrameLib_GreaterThan,
FrameLib_LessThan,
FrameLib_GreaterThanEqual,
FrameLib_LessThanEqual,
FrameLib_LogicalAnd,
FrameLib_LogicalOr,

FrameLib_Pow,
FrameLib_Atan2,
FrameLib_Hypot,
FrameLib_CopySign, 
FrameLib_Min,
FrameLib_Max,
FrameLib_Diff,
FrameLib_Modulo,

// Ternary Operators

FrameLib_Clip,
FrameLib_Fold,
FrameLib_Wrap,

// Expressions

FrameLib_Expression,
FrameLib_ComplexExpression,

// Complex Unary Operators

FrameLib_Complex_Cos,
FrameLib_Complex_Sin,
FrameLib_Complex_Tan,
FrameLib_Complex_Cosh,
FrameLib_Complex_Sinh,
FrameLib_Complex_Tanh,

FrameLib_Complex_Log,
FrameLib_Complex_Log10,
FrameLib_Complex_Exp,

FrameLib_Complex_Sqrt,
FrameLib_Complex_Conj,

FrameLib_Cartopol,
FrameLib_Poltocar,

// Complex Binary Operators

FrameLib_Complex_Plus,
FrameLib_Complex_Minus,
FrameLib_Complex_Multiply,
FrameLib_Complex_Divide,
FrameLib_Complex_Pow,

// Buffer

FrameLib_Info,
FrameLib_Read
>;

#endif /* FrameLib_TypeList_h */
