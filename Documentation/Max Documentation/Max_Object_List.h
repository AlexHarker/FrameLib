#include "FrameLib_TypeList.h"
 
using FrameLib_DSPList = detail::FrameLib_Typelist<
 
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
FrameLib_Expand<FrameLib_VectorMin>,
FrameLib_Expand<FrameLib_Sum>,
FrameLib_Expand<FrameLib_Subframe>,
FrameLib_Expand<FrameLib_NanFilter>,
FrameLib_Expand<FrameLib_Reverse>,
FrameLib_Expand<FrameLib_Length>,
FrameLib_Expand<FrameLib_Peaks>,
FrameLib_Expand<FrameLib_Pattern>,
FrameLib_Expand<FrameLib_Product>,
FrameLib_Expand<FrameLib_Skewness>,
FrameLib_Expand<FrameLib_Crest>,
FrameLib_Expand<FrameLib_Pad>,
FrameLib_Expand<FrameLib_Chop>,
FrameLib_Expand<FrameLib_Flatness>,
FrameLib_Expand<FrameLib_RMS>,
FrameLib_Expand<FrameLib_Split>,
FrameLib_Expand<FrameLib_MedianFilter>

 
>;
 
template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_LessThan>>::name()
{ return "fl.lessthan"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Atan2>>::name()
{ return "fl.atan2"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Minus>>::name()
{ return "fl.minus"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Hypot>>::name()
{ return "fl.hypot"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Equal>>::name()
{ return "fl.equals"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Multiply>>::name()
{ return "fl.times"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Pow>>::name()
{ return "fl.pow"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Divide>>::name()
{ return "fl.divide"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Max>>::name()
{ return "fl.max"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_NotEqual>>::name()
{ return "fl.notequals"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_GreaterThan>>::name()
{ return "fl.greaterthan"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_LogicalAnd>>::name()
{ return "fl.and"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Min>>::name()
{ return "fl.min"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_LessThanEqual>>::name()
{ return "fl.lessthaneq"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Diff>>::name()
{ return "fl.diff"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Plus>>::name()
{ return "fl.plus"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_LogicalOr>>::name()
{ return "fl.or"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_CopySign>>::name()
{ return "fl.copysign"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_GreaterThanEqual>>::name()
{ return "fl.greaterthaneq"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Modulo>>::name()
{ return "fl.modulo"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Info>>::name()
{ return "fl.info"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Read>>::name()
{ return "fl.read"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Pow>>::name()
{ return "fl.complexpow"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Minus>>::name()
{ return "fl.complexminus"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Plus>>::name()
{ return "fl.complexplus"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Divide>>::name()
{ return "fl.complexdivide"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Multiply>>::name()
{ return "fl.complexmultiply"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Cos>>::name()
{ return "fl.complexcos"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Log10>>::name()
{ return "fl.complexlog10"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Sinh>>::name()
{ return "fl.complexsinh"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Cartopol>>::name()
{ return "fl.cartopol"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Conj>>::name()
{ return "fl.complexconj"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Exp>>::name()
{ return "fl.complexexp"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Log>>::name()
{ return "fl.complexlog"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Cosh>>::name()
{ return "fl.complexcosh"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Tan>>::name()
{ return "fl.complextan"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Sin>>::name()
{ return "fl.complexsin"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Tanh>>::name()
{ return "fl.complextanh"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Complex_Sqrt>>::name()
{ return "fl.complexsqrt"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Poltocar>>::name()
{ return "fl.poltocar"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_ComplexExpression>>::name()
{ return "fl.complexexpr"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Expression>>::name()
{ return "fl.expr"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Resonant>>::name()
{ return "fl.resonant"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_0dfSVF>>::name()
{ return "fl.0dfsvf"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_SallenAndKey>>::name()
{ return "fl.sallenkey"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_OnePoleZero>>::name()
{ return "fl.onepolezero"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_OnePole>>::name()
{ return "fl.onepole"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Random>>::name()
{ return "fl.random"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Gaussian>>::name()
{ return "fl.gaussian"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Uniform>>::name()
{ return "fl.uniform"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Ramp>>::name()
{ return "fl.ramp"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_ToHost>>::name()
{ return "fl.tomax"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_FromHost>>::name()
{ return "fl.frommax"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Source>>::name()
{ return "fl.source"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Trace>>::name()
{ return "fl.trace"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Sink>>::name()
{ return "fl.sink"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Convert>>::name()
{ return "fl.convert"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Map>>::name()
{ return "fl.map"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Constant>>::name()
{ return "fl.constant"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_SampleRate>>::name()
{ return "fl.samplerate"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Lookup>>::name()
{ return "fl.lookup"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Tag>>::name()
{ return "fl.tag"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Untag>>::name()
{ return "fl.untag"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_CombineTags>>::name()
{ return "fl.combinetags"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_GetParam>>::name()
{ return "fl.getparam"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_FilterTags>>::name()
{ return "fl.filtertags"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Dispatch>>::name()
{ return "fl.dispatch"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Route>>::name()
{ return "fl.route"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Select>>::name()
{ return "fl.select"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Interval>>::name()
{ return "fl.interval"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_PerBlock>>::name()
{ return "fl.perblock"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_AudioTrigger>>::name()
{ return "fl.audiotrigger"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Once>>::name()
{ return "fl.once"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Chain>>::name()
{ return "fl.chain"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Future>>::name()
{ return "fl.future"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_CoordinateSystem>>::name()
{ return "fl.coordinatesystem"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Spatial>>::name()
{ return "fl.spatial"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_FFT>>::name()
{ return "fl.fft"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Multitaper>>::name()
{ return "fl.multitaper"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Convolve>>::name()
{ return "fl.convolve"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_iFFT>>::name()
{ return "fl.ifft"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Correlate>>::name()
{ return "fl.correlate"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Window>>::name()
{ return "fl.window"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Store>>::name()
{ return "fl.store"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Register>>::name()
{ return "fl.register"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Recall>>::name()
{ return "fl.recall"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Pack>::name()
{ return "fl.pack"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Unpack>::name()
{ return "fl.unpack"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_StreamID>>::name()
{ return "fl.streamid"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Wrap>>::name()
{ return "fl.wrap"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Clip>>::name()
{ return "fl.clip"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Fold>>::name()
{ return "fl.fold"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_TimeStdDev>>::name()
{ return "fl.timestddev"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_EWMA>>::name()
{ return "fl.ewma"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_TimeMedian>>::name()
{ return "fl.timemedian"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Lag>>::name()
{ return "fl.lag"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_EWMSD>>::name()
{ return "fl.ewmsd"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_FrameDelta>>::name()
{ return "fl.framedelta"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_TimeMean>>::name()
{ return "fl.timemean"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_TimeDelta>>::name()
{ return "fl.timedelta"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Now>>::name()
{ return "fl.now"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Timer>>::name()
{ return "fl.timer"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Ticks>>::name()
{ return "fl.ticks"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Sqrt>>::name()
{ return "fl.sqrt"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Floor>>::name()
{ return "fl.floor"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Exp2>>::name()
{ return "fl.exp2"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Tanh>>::name()
{ return "fl.tanh"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Cbrt>>::name()
{ return "fl.cbrt"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Cos>>::name()
{ return "fl.cos"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Atanh>>::name()
{ return "fl.atanh"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Acosh>>::name()
{ return "fl.acosh"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Atan>>::name()
{ return "fl.atan"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Erf>>::name()
{ return "fl.erf"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Cosh>>::name()
{ return "fl.cosh"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Asin>>::name()
{ return "fl.asin"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Exp>>::name()
{ return "fl.exp"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Log10>>::name()
{ return "fl.log10"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Log2>>::name()
{ return "fl.log2"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Erfc>>::name()
{ return "fl.erfc"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Acos>>::name()
{ return "fl.acos"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Abs>>::name()
{ return "fl.abs"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Log>>::name()
{ return "fl.log"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Asinh>>::name()
{ return "fl.asinh"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Trunc>>::name()
{ return "fl.trunc"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_LogicalNot>>::name()
{ return "fl.not"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Sinh>>::name()
{ return "fl.sinh"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Ceil>>::name()
{ return "fl.ceil"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Round>>::name()
{ return "fl.round"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Tan>>::name()
{ return "fl.tan"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Sin>>::name()
{ return "fl.sin"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_VectorArgMin>>::name()
{ return "fl.argmin"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Percentile>>::name()
{ return "fl.percentile"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_StandardDeviation>>::name()
{ return "fl.standarddeviation"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Centroid>>::name()
{ return "fl.centroid"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_AccumPoint>>::name()
{ return "fl.accumpoint"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_NonZero>>::name()
{ return "fl.nonzero"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_VectorArgMax>>::name()
{ return "fl.argmax"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_GeometricMean>>::name()
{ return "fl.geometricmean"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Spread>>::name()
{ return "fl.spread"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_VectorMax>>::name()
{ return "fl.vmax"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Kurtosis>>::name()
{ return "fl.kurtosis"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Sort>>::name()
{ return "fl.sort"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Mean>>::name()
{ return "fl.mean"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Join>>::name()
{ return "fl.join"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Shift>>::name()
{ return "fl.shift"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_VectorMin>>::name()
{ return "fl.vmin"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Sum>>::name()
{ return "fl.sum"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Subframe>>::name()
{ return "fl.subframe"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_NanFilter>>::name()
{ return "fl.nanfilter"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Reverse>>::name()
{ return "fl.reverse"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Length>>::name()
{ return "fl.length"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Peaks>>::name()
{ return "fl.peaks"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Pattern>>::name()
{ return "fl.pattern"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Product>>::name()
{ return "fl.product"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Skewness>>::name()
{ return "fl.skewness"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Crest>>::name()
{ return "fl.crest"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Pad>>::name()
{ return "fl.pad"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Chop>>::name()
{ return "fl.chop"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Flatness>>::name()
{ return "fl.flatness"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_RMS>>::name()
{ return "fl.rms"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_Split>>::name()
{ return "fl.split"; }

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_MedianFilter>>::name()
{ return "fl.medianfilter"; }

