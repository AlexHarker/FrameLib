
#ifndef FRAMELIB_OBJECTS_H
#define FRAMELIB_OBJECTS_H

// Buffers

#include "../FrameLib_Objects/Buffer/FrameLib_Info.h"
#include "../FrameLib_Objects/Buffer/FrameLib_Read.h"

// Host Communication

#include "../FrameLib_Objects/Host_Communication/FrameLib_FromHost.h"
#include "../FrameLib_Objects/Host_Communication/FrameLib_ToHost.h"

// Filters

#include "../FrameLib_Objects/Filters/FrameLib_Biquad.h"
#include "../FrameLib_Objects/Filters/FrameLib_BiquadCoeff.h"
#include "../FrameLib_Objects/Filters/FrameLib_OnePole.h"
#include "../FrameLib_Objects/Filters/FrameLib_OnePoleZero.h"
#include "../FrameLib_Objects/Filters/FrameLib_SVF.h"

// Generators

#include "../FrameLib_Objects/Generators/FrameLib_Gaussian.h"
#include "../FrameLib_Objects/Generators/FrameLib_MakeWindow.h"
#include "../FrameLib_Objects/Generators/FrameLib_Ramp.h"
#include "../FrameLib_Objects/Generators/FrameLib_Random.h"
#include "../FrameLib_Objects/Generators/FrameLib_Uniform.h"

// IO

#include "../FrameLib_Objects/IO/FrameLib_Source.h"
#include "../FrameLib_Objects/IO/FrameLib_Sink.h"
#include "../FrameLib_Objects/IO/FrameLib_Trace.h"

// Mapping

#include "../FrameLib_Objects/Mapping/FrameLib_Constant.h"
#include "../FrameLib_Objects/Mapping/FrameLib_Convert.h"
#include "../FrameLib_Objects/Mapping/FrameLib_Lookup.h"
#include "../FrameLib_Objects/Mapping/FrameLib_Map.h"
#include "../FrameLib_Objects/Mapping/FrameLib_SampleRate.h"

// Parameters

#include "../FrameLib_Objects/Parameters/FrameLib_CombineTags.h"
#include "../FrameLib_Objects/Parameters/FrameLib_FilterTags.h"
#include "../FrameLib_Objects/Parameters/FrameLib_LookupString.h"
#include "../FrameLib_Objects/Parameters/FrameLib_MakeString.h"
#include "../FrameLib_Objects/Parameters/FrameLib_Tag.h"
#include "../FrameLib_Objects/Parameters/FrameLib_Untag.h"

// Routing

#include "../FrameLib_Objects/Routing/FrameLib_Change.h"
#include "../FrameLib_Objects/Routing/FrameLib_Dispatch.h"
#include "../FrameLib_Objects/Routing/FrameLib_Route.h"
#include "../FrameLib_Objects/Routing/FrameLib_Select.h"

// Schedulers

#include "../FrameLib_Objects/Schedulers/FrameLib_AudioTrigger.h"
#include "../FrameLib_Objects/Schedulers/FrameLib_Chain.h"
#include "../FrameLib_Objects/Schedulers/FrameLib_Interval.h"
#include "../FrameLib_Objects/Schedulers/FrameLib_Once.h"
#include "../FrameLib_Objects/Schedulers/FrameLib_PerBlock.h"
#include "../FrameLib_Objects/Schedulers/FrameLib_Recursive.h"

// Spatial

#include "../FrameLib_Objects/Spatial/FrameLib_Cartopol.h"
#include "../FrameLib_Objects/Spatial/FrameLib_Poltocar.h"
#include "../FrameLib_Objects/Spatial/FrameLib_Spatial.h"

// Spectral

#include "../FrameLib_Objects/Spectral/FrameLib_FFT.h"
#include "../FrameLib_Objects/Spectral/FrameLib_iFFT.h"
#include "../FrameLib_Objects/Spectral/FrameLib_Correlate.h"
#include "../FrameLib_Objects/Spectral/FrameLib_Convolve.h"
#include "../FrameLib_Objects/Spectral/FrameLib_FIRPhase.h"
#include "../FrameLib_Objects/Spectral/FrameLib_Multitaper.h"
#include "../FrameLib_Objects/Spectral/FrameLib_Window.h"

// Storage

#include "../FrameLib_Objects/Storage/FrameLib_Recall.h"
#include "../FrameLib_Objects/Storage/FrameLib_Register.h"
#include "../FrameLib_Objects/Storage/FrameLib_Store.h"

// Streaming

#include "../FrameLib_Objects/Streaming/FrameLib_Pack.h"
#include "../FrameLib_Objects/Streaming/FrameLib_Unpack.h"
#include "../FrameLib_Objects/Streaming/FrameLib_StreamID.h"

// Time Smoothing

#include "../FrameLib_Objects/Time_Smoothing/FrameLib_FrameDelta.h"
#include "../FrameLib_Objects/Time_Smoothing/FrameLib_Lag.h"
#include "../FrameLib_Objects/Time_Smoothing/FrameLib_MovingAverage.h"
#include "../FrameLib_Objects/Time_Smoothing/FrameLib_TimeMean.h"
#include "../FrameLib_Objects/Time_Smoothing/FrameLib_TimeMedian.h"
#include "../FrameLib_Objects/Time_Smoothing/FrameLib_TimeStdDev.h"

// Timing

#include "../FrameLib_Objects/Timing/FrameLib_Now.h"
#include "../FrameLib_Objects/Timing/FrameLib_Ticks.h"
#include "../FrameLib_Objects/Timing/FrameLib_TimeDelta.h"
#include "../FrameLib_Objects/Timing/FrameLib_Timer.h"

// Vector

#include "../FrameLib_Objects/Vector/FrameLib_AccumPos.h"
#include "../FrameLib_Objects/Vector/FrameLib_Chop.h"
#include "../FrameLib_Objects/Vector/FrameLib_Cumulative.h"
#include "../FrameLib_Objects/Vector/FrameLib_Join.h"
#include "../FrameLib_Objects/Vector/FrameLib_KernelSmooth.h"
#include "../FrameLib_Objects/Vector/FrameLib_MedianFilter.h"
#include "../FrameLib_Objects/Vector/FrameLib_NanFilter.h"
#include "../FrameLib_Objects/Vector/FrameLib_NonZero.h"
#include "../FrameLib_Objects/Vector/FrameLib_Pad.h"
#include "../FrameLib_Objects/Vector/FrameLib_Paste.h"
#include "../FrameLib_Objects/Vector/FrameLib_Pattern.h"
#include "../FrameLib_Objects/Vector/FrameLib_Peaks.h"
#include "../FrameLib_Objects/Vector/FrameLib_Percentile.h"
#include "../FrameLib_Objects/Vector/FrameLib_Prioritise.h"
#include "../FrameLib_Objects/Vector/FrameLib_Reverse.h"
#include "../FrameLib_Objects/Vector/FrameLib_Shift.h"
#include "../FrameLib_Objects/Vector/FrameLib_Sort.h"
#include "../FrameLib_Objects/Vector/FrameLib_SortMultiple.h"
#include "../FrameLib_Objects/Vector/FrameLib_Split.h"
#include "../FrameLib_Objects/Vector/FrameLib_Subframe.h"
#include "../FrameLib_Objects/Vector/FrameLib_Vector_Objects.h"

// Operators

#include "../FrameLib_Objects/Unary/FrameLib_Unary_Objects.h"
#include "../FrameLib_Objects/Binary/FrameLib_Binary_Objects.h"
#include "../FrameLib_Objects/Ternary/FrameLib_Ternary_Objects.h"

// Expressions

#include "../FrameLib_Objects/Expressions/FrameLib_Expression.h"
#include "../FrameLib_Objects/Expressions/FrameLib_ComplexExpression.h"

// Complex Operators

#include "../FrameLib_Objects/Complex_Unary/FrameLib_Complex_Unary_Objects.h"
#include "../FrameLib_Objects/Complex_Binary/FrameLib_Complex_Binary_Objects.h"

#endif /* FRAMELIB_OBJECTS_H */
