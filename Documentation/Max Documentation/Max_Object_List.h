#include "FrameLib_TypeList.h"
 
using FrameLib_DSPList = FrameLib_Typelist<
 
FrameLib_Expand<FrameLib_LessThan>,
FrameLib_Expand<FrameLib_Atan2>,
FrameLib_Expand<FrameLib_Minus>,
FrameLib_Expand<FrameLib_Hypot>,
FrameLib_Expand<FrameLib_Equal>,
FrameLib_Expand<FrameLib_Multiply>,
FrameLib_Expand<FrameLib_Pow>,
FrameLib_Expand<FrameLib_Divide>,
FrameLib_Expand<FrameLib_Max>,
FrameLib_Expand<FrameLib_NotEqual>,
FrameLib_Expand<FrameLib_GreaterThan>,
FrameLib_Expand<FrameLib_LogicalAnd>,
FrameLib_Expand<FrameLib_Min>,
FrameLib_Expand<FrameLib_LessThanEqual>,
FrameLib_Expand<FrameLib_Diff>,
FrameLib_Expand<FrameLib_Plus>,
FrameLib_Expand<FrameLib_LogicalOr>,
FrameLib_Expand<FrameLib_CopySign>,
FrameLib_Expand<FrameLib_GreaterThanEqual>,
FrameLib_Expand<FrameLib_Modulo>,
FrameLib_Expand<FrameLib_Info>,
FrameLib_Expand<FrameLib_Read>,
FrameLib_Expand<FrameLib_Complex_Pow>,
FrameLib_Expand<FrameLib_Complex_Minus>,
FrameLib_Expand<FrameLib_Complex_Plus>,
FrameLib_Expand<FrameLib_Complex_Divide>,
FrameLib_Expand<FrameLib_Complex_Multiply>,
FrameLib_Expand<FrameLib_Complex_Cos>,
FrameLib_Expand<FrameLib_Complex_Log10>,
FrameLib_Expand<FrameLib_Complex_Sinh>,
FrameLib_Expand<FrameLib_Cartopol>,
FrameLib_Expand<FrameLib_Complex_Conj>,
FrameLib_Expand<FrameLib_Complex_Exp>,
FrameLib_Expand<FrameLib_Complex_Log>,
FrameLib_Expand<FrameLib_Complex_Cosh>,
FrameLib_Expand<FrameLib_Complex_Tan>,
FrameLib_Expand<FrameLib_Complex_Sin>,
FrameLib_Expand<FrameLib_Complex_Tanh>,
FrameLib_Expand<FrameLib_Complex_Sqrt>,
FrameLib_Expand<FrameLib_Poltocar>,
FrameLib_Expand<FrameLib_ComplexExpression>,
FrameLib_Expand<FrameLib_Expression>,
FrameLib_Expand<FrameLib_Resonant>,
FrameLib_Expand<FrameLib_0dfSVF>,
FrameLib_Expand<FrameLib_SallenAndKey>,
FrameLib_Expand<FrameLib_OnePoleZero>,
FrameLib_Expand<FrameLib_OnePole>,
FrameLib_Expand<FrameLib_Random>,
FrameLib_Expand<FrameLib_Gaussian>,
FrameLib_Expand<FrameLib_Uniform>,
FrameLib_Expand<FrameLib_Ramp>,
FrameLib_Expand<FrameLib_ToHost>,
FrameLib_Expand<FrameLib_FromHost>,
FrameLib_Expand<FrameLib_Source>,
FrameLib_Expand<FrameLib_Trace>,
FrameLib_Expand<FrameLib_Sink>,
FrameLib_Expand<FrameLib_Convert>,
FrameLib_Expand<FrameLib_Map>,
FrameLib_Expand<FrameLib_Constant>,
FrameLib_Expand<FrameLib_SampleRate>,
FrameLib_Expand<FrameLib_Lookup>,
FrameLib_Expand<FrameLib_Tag>,
FrameLib_Expand<FrameLib_Untag>,
FrameLib_Expand<FrameLib_CombineTags>,
FrameLib_Expand<FrameLib_GetParam>,
FrameLib_Expand<FrameLib_FilterTags>,
FrameLib_Expand<FrameLib_Dispatch>,
FrameLib_Expand<FrameLib_Route>,
FrameLib_Expand<FrameLib_Select>,
FrameLib_Expand<FrameLib_Interval>,
FrameLib_Expand<FrameLib_PerBlock>,
FrameLib_Expand<FrameLib_AudioTrigger>,
FrameLib_Expand<FrameLib_Once>,
FrameLib_Expand<FrameLib_Chain>,
FrameLib_Expand<FrameLib_Future>,
FrameLib_Expand<FrameLib_CoordinateSystem>,
FrameLib_Expand<FrameLib_Spatial>,
FrameLib_Expand<FrameLib_FFT>,
FrameLib_Expand<FrameLib_Multitaper>,
FrameLib_Expand<FrameLib_Convolve>,
FrameLib_Expand<FrameLib_iFFT>,
FrameLib_Expand<FrameLib_Correlate>,
FrameLib_Expand<FrameLib_Window>,
FrameLib_Expand<FrameLib_Store>,
FrameLib_Expand<FrameLib_Register>,
FrameLib_Expand<FrameLib_Recall>,
FrameLib_Pack,
FrameLib_Unpack,
FrameLib_Expand<FrameLib_StreamID>,
FrameLib_Expand<FrameLib_Wrap>,
FrameLib_Expand<FrameLib_Clip>,
FrameLib_Expand<FrameLib_Fold>,
FrameLib_Expand<FrameLib_TimeStdDev>,
FrameLib_Expand<FrameLib_EWMA>,
FrameLib_Expand<FrameLib_TimeMedian>,
FrameLib_Expand<FrameLib_Lag>,
FrameLib_Expand<FrameLib_EWMSD>,
FrameLib_Expand<FrameLib_FrameDelta>,
FrameLib_Expand<FrameLib_TimeMean>,
FrameLib_Expand<FrameLib_TimeDelta>,
FrameLib_Expand<FrameLib_Now>,
FrameLib_Expand<FrameLib_Timer>,
FrameLib_Expand<FrameLib_Ticks>,
FrameLib_Expand<FrameLib_Sqrt>,
FrameLib_Expand<FrameLib_Floor>,
FrameLib_Expand<FrameLib_Exp2>,
FrameLib_Expand<FrameLib_Tanh>,
FrameLib_Expand<FrameLib_Cbrt>,
FrameLib_Expand<FrameLib_Cos>,
FrameLib_Expand<FrameLib_Atanh>,
FrameLib_Expand<FrameLib_Acosh>,
FrameLib_Expand<FrameLib_Atan>,
FrameLib_Expand<FrameLib_Erf>,
FrameLib_Expand<FrameLib_Cosh>,
FrameLib_Expand<FrameLib_Asin>,
FrameLib_Expand<FrameLib_Exp>,
FrameLib_Expand<FrameLib_Log10>,
FrameLib_Expand<FrameLib_Log2>,
FrameLib_Expand<FrameLib_Erfc>,
FrameLib_Expand<FrameLib_Acos>,
FrameLib_Expand<FrameLib_Abs>,
FrameLib_Expand<FrameLib_Log>,
FrameLib_Expand<FrameLib_Asinh>,
FrameLib_Expand<FrameLib_Trunc>,
FrameLib_Expand<FrameLib_LogicalNot>,
FrameLib_Expand<FrameLib_Sinh>,
FrameLib_Expand<FrameLib_Ceil>,
FrameLib_Expand<FrameLib_Round>,
FrameLib_Expand<FrameLib_Tan>,
FrameLib_Expand<FrameLib_Sin>,
FrameLib_Expand<FrameLib_VectorArgMin>,
FrameLib_Expand<FrameLib_Percentile>,
FrameLib_Expand<FrameLib_StandardDeviation>,
FrameLib_Expand<FrameLib_Centroid>,
FrameLib_Expand<FrameLib_AccumPoint>,
FrameLib_Expand<FrameLib_NonZero>,
FrameLib_Expand<FrameLib_VectorArgMax>,
FrameLib_Expand<FrameLib_GeometricMean>,
FrameLib_Expand<FrameLib_Spread>,
FrameLib_Expand<FrameLib_VectorMax>,
FrameLib_Expand<FrameLib_Kurtosis>,
FrameLib_Expand<FrameLib_Sort>,
FrameLib_Expand<FrameLib_Mean>,
FrameLib_Expand<FrameLib_Join>,
FrameLib_Expand<FrameLib_Shift>,
FrameLib_Expand<FrameLib_Closest>,
FrameLib_Expand<FrameLib_VectorMin>,
FrameLib_Expand<FrameLib_Sum>,
FrameLib_Expand<FrameLib_Subframe>,
FrameLib_Expand<FrameLib_NanFilter>,
FrameLib_Expand<FrameLib_Reverse>,
FrameLib_Expand<FrameLib_Length>,
FrameLib_Expand<FrameLib_Peaks>,
FrameLib_Expand<FrameLib_Pattern>,
FrameLib_Expand<FrameLib_Scramble>,
FrameLib_Expand<FrameLib_Product>,
FrameLib_Expand<FrameLib_Skewness>,
FrameLib_Expand<FrameLib_Crest>,
FrameLib_Expand<FrameLib_Pad>,
FrameLib_Expand<FrameLib_Find>,
FrameLib_Expand<FrameLib_Chop>,
FrameLib_Expand<FrameLib_Flatness>,
FrameLib_Expand<FrameLib_RMS>,
FrameLib_Expand<FrameLib_Split>,
FrameLib_Expand<FrameLib_MedianFilter>

 
>:
 
template<>
 
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_LessThan}>>::name() { return "{FrameLib_LessThan}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Atan2}>>::name() { return "{FrameLib_Atan2}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Minus}>>::name() { return "{FrameLib_Minus}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Hypot}>>::name() { return "{FrameLib_Hypot}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Equal}>>::name() { return "{FrameLib_Equal}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Multiply}>>::name() { return "{FrameLib_Multiply}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Pow}>>::name() { return "{FrameLib_Pow}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Divide}>>::name() { return "{FrameLib_Divide}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Max}>>::name() { return "{FrameLib_Max}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_NotEqual}>>::name() { return "{FrameLib_NotEqual}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_GreaterThan}>>::name() { return "{FrameLib_GreaterThan}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_LogicalAnd}>>::name() { return "{FrameLib_LogicalAnd}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Min}>>::name() { return "{FrameLib_Min}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_LessThanEqual}>>::name() { return "{FrameLib_LessThanEqual}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Diff}>>::name() { return "{FrameLib_Diff}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Plus}>>::name() { return "{FrameLib_Plus}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_LogicalOr}>>::name() { return "{FrameLib_LogicalOr}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_CopySign}>>::name() { return "{FrameLib_CopySign}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_GreaterThanEqual}>>::name() { return "{FrameLib_GreaterThanEqual}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Modulo}>>::name() { return "{FrameLib_Modulo}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Info}>>::name() { return "{FrameLib_Info}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Read}>>::name() { return "{FrameLib_Read}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Pow}>>::name() { return "{FrameLib_Complex_Pow}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Minus}>>::name() { return "{FrameLib_Complex_Minus}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Plus}>>::name() { return "{FrameLib_Complex_Plus}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Divide}>>::name() { return "{FrameLib_Complex_Divide}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Multiply}>>::name() { return "{FrameLib_Complex_Multiply}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Cos}>>::name() { return "{FrameLib_Complex_Cos}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Log10}>>::name() { return "{FrameLib_Complex_Log10}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Sinh}>>::name() { return "{FrameLib_Complex_Sinh}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Cartopol}>>::name() { return "{FrameLib_Cartopol}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Conj}>>::name() { return "{FrameLib_Complex_Conj}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Exp}>>::name() { return "{FrameLib_Complex_Exp}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Log}>>::name() { return "{FrameLib_Complex_Log}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Cosh}>>::name() { return "{FrameLib_Complex_Cosh}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Tan}>>::name() { return "{FrameLib_Complex_Tan}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Sin}>>::name() { return "{FrameLib_Complex_Sin}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Tanh}>>::name() { return "{FrameLib_Complex_Tanh}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Complex_Sqrt}>>::name() { return "{FrameLib_Complex_Sqrt}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Poltocar}>>::name() { return "{FrameLib_Poltocar}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_ComplexExpression}>>::name() { return "{FrameLib_ComplexExpression}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Expression}>>::name() { return "{FrameLib_Expression}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Resonant}>>::name() { return "{FrameLib_Resonant}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_0dfSVF}>>::name() { return "{FrameLib_0dfSVF}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_SallenAndKey}>>::name() { return "{FrameLib_SallenAndKey}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_OnePoleZero}>>::name() { return "{FrameLib_OnePoleZero}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_OnePole}>>::name() { return "{FrameLib_OnePole}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Random}>>::name() { return "{FrameLib_Random}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Gaussian}>>::name() { return "{FrameLib_Gaussian}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Uniform}>>::name() { return "{FrameLib_Uniform}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Ramp}>>::name() { return "{FrameLib_Ramp}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_ToHost}>>::name() { return "{FrameLib_ToHost}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_FromHost}>>::name() { return "{FrameLib_FromHost}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Source}>>::name() { return "{FrameLib_Source}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Trace}>>::name() { return "{FrameLib_Trace}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Sink}>>::name() { return "{FrameLib_Sink}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Convert}>>::name() { return "{FrameLib_Convert}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Map}>>::name() { return "{FrameLib_Map}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Constant}>>::name() { return "{FrameLib_Constant}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_SampleRate}>>::name() { return "{FrameLib_SampleRate}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Lookup}>>::name() { return "{FrameLib_Lookup}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Tag}>>::name() { return "{FrameLib_Tag}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Untag}>>::name() { return "{FrameLib_Untag}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_CombineTags}>>::name() { return "{FrameLib_CombineTags}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_GetParam}>>::name() { return "{FrameLib_GetParam}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_FilterTags}>>::name() { return "{FrameLib_FilterTags}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Dispatch}>>::name() { return "{FrameLib_Dispatch}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Route}>>::name() { return "{FrameLib_Route}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Select}>>::name() { return "{FrameLib_Select}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Interval}>>::name() { return "{FrameLib_Interval}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_PerBlock}>>::name() { return "{FrameLib_PerBlock}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_AudioTrigger}>>::name() { return "{FrameLib_AudioTrigger}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Once}>>::name() { return "{FrameLib_Once}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Chain}>>::name() { return "{FrameLib_Chain}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Future}>>::name() { return "{FrameLib_Future}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_CoordinateSystem}>>::name() { return "{FrameLib_CoordinateSystem}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Spatial}>>::name() { return "{FrameLib_Spatial}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_FFT}>>::name() { return "{FrameLib_FFT}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Multitaper}>>::name() { return "{FrameLib_Multitaper}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Convolve}>>::name() { return "{FrameLib_Convolve}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_iFFT}>>::name() { return "{FrameLib_iFFT}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Correlate}>>::name() { return "{FrameLib_Correlate}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Window}>>::name() { return "{FrameLib_Window}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Store}>>::name() { return "{FrameLib_Store}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Register}>>::name() { return "{FrameLib_Register}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Recall}>>::name() { return "{FrameLib_Recall}"; }
const char* FrameLib_ObjectName<{FrameLib_Pack}>::name() { return "{FrameLib_Pack}"; }
const char* FrameLib_ObjectName<{FrameLib_Unpack}>::name() { return "{FrameLib_Unpack}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_StreamID}>>::name() { return "{FrameLib_StreamID}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Wrap}>>::name() { return "{FrameLib_Wrap}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Clip}>>::name() { return "{FrameLib_Clip}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Fold}>>::name() { return "{FrameLib_Fold}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_TimeStdDev}>>::name() { return "{FrameLib_TimeStdDev}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_EWMA}>>::name() { return "{FrameLib_EWMA}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_TimeMedian}>>::name() { return "{FrameLib_TimeMedian}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Lag}>>::name() { return "{FrameLib_Lag}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_EWMSD}>>::name() { return "{FrameLib_EWMSD}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_FrameDelta}>>::name() { return "{FrameLib_FrameDelta}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_TimeMean}>>::name() { return "{FrameLib_TimeMean}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_TimeDelta}>>::name() { return "{FrameLib_TimeDelta}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Now}>>::name() { return "{FrameLib_Now}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Timer}>>::name() { return "{FrameLib_Timer}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Ticks}>>::name() { return "{FrameLib_Ticks}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Sqrt}>>::name() { return "{FrameLib_Sqrt}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Floor}>>::name() { return "{FrameLib_Floor}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Exp2}>>::name() { return "{FrameLib_Exp2}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Tanh}>>::name() { return "{FrameLib_Tanh}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Cbrt}>>::name() { return "{FrameLib_Cbrt}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Cos}>>::name() { return "{FrameLib_Cos}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Atanh}>>::name() { return "{FrameLib_Atanh}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Acosh}>>::name() { return "{FrameLib_Acosh}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Atan}>>::name() { return "{FrameLib_Atan}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Erf}>>::name() { return "{FrameLib_Erf}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Cosh}>>::name() { return "{FrameLib_Cosh}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Asin}>>::name() { return "{FrameLib_Asin}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Exp}>>::name() { return "{FrameLib_Exp}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Log10}>>::name() { return "{FrameLib_Log10}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Log2}>>::name() { return "{FrameLib_Log2}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Erfc}>>::name() { return "{FrameLib_Erfc}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Acos}>>::name() { return "{FrameLib_Acos}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Abs}>>::name() { return "{FrameLib_Abs}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Log}>>::name() { return "{FrameLib_Log}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Asinh}>>::name() { return "{FrameLib_Asinh}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Trunc}>>::name() { return "{FrameLib_Trunc}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_LogicalNot}>>::name() { return "{FrameLib_LogicalNot}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Sinh}>>::name() { return "{FrameLib_Sinh}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Ceil}>>::name() { return "{FrameLib_Ceil}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Round}>>::name() { return "{FrameLib_Round}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Tan}>>::name() { return "{FrameLib_Tan}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Sin}>>::name() { return "{FrameLib_Sin}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_VectorArgMin}>>::name() { return "{FrameLib_VectorArgMin}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Percentile}>>::name() { return "{FrameLib_Percentile}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_StandardDeviation}>>::name() { return "{FrameLib_StandardDeviation}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Centroid}>>::name() { return "{FrameLib_Centroid}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_AccumPoint}>>::name() { return "{FrameLib_AccumPoint}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_NonZero}>>::name() { return "{FrameLib_NonZero}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_VectorArgMax}>>::name() { return "{FrameLib_VectorArgMax}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_GeometricMean}>>::name() { return "{FrameLib_GeometricMean}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Spread}>>::name() { return "{FrameLib_Spread}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_VectorMax}>>::name() { return "{FrameLib_VectorMax}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Kurtosis}>>::name() { return "{FrameLib_Kurtosis}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Sort}>>::name() { return "{FrameLib_Sort}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Mean}>>::name() { return "{FrameLib_Mean}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Join}>>::name() { return "{FrameLib_Join}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Shift}>>::name() { return "{FrameLib_Shift}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Closest}>>::name() { return "{FrameLib_Closest}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_VectorMin}>>::name() { return "{FrameLib_VectorMin}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Sum}>>::name() { return "{FrameLib_Sum}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Subframe}>>::name() { return "{FrameLib_Subframe}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_NanFilter}>>::name() { return "{FrameLib_NanFilter}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Reverse}>>::name() { return "{FrameLib_Reverse}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Length}>>::name() { return "{FrameLib_Length}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Peaks}>>::name() { return "{FrameLib_Peaks}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Pattern}>>::name() { return "{FrameLib_Pattern}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Scramble}>>::name() { return "{FrameLib_Scramble}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Product}>>::name() { return "{FrameLib_Product}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Skewness}>>::name() { return "{FrameLib_Skewness}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Crest}>>::name() { return "{FrameLib_Crest}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Pad}>>::name() { return "{FrameLib_Pad}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Find}>>::name() { return "{FrameLib_Find}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Chop}>>::name() { return "{FrameLib_Chop}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Flatness}>>::name() { return "{FrameLib_Flatness}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_RMS}>>::name() { return "{FrameLib_RMS}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_Split}>>::name() { return "{FrameLib_Split}"; }
const char* FrameLib_ObjectName<FrameLib_Expand<{FrameLib_MedianFilter}>>::name() { return "{FrameLib_MedianFilter}"; }
