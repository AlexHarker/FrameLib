
#ifndef FRAMELIB_OBJECTS_H
#define FRAMELIB_OBJECTS_H

// Filters

#include "FrameLib_0dfSVF.h"
#include "FrameLib_OnePole.h"
#include "FrameLib_OnePoleZero.h"
#include "FrameLib_Resonant.h"
#include "FrameLib_SallenAndKey.h"

// Generators

#include "FrameLib_Gaussian.h"
#include "FrameLib_Ramp.h"
#include "FrameLib_Random.h"
#include "FrameLib_Uniform.h"

// IO

#include "FrameLib_Source.h"
#include "FrameLib_Sink.h"
#include "FrameLib_Trace.h"

// Mapping

#include "FrameLib_Constant.h"
#include "FrameLib_Convert.h"
#include "FrameLib_Lookup.h"
#include "FrameLib_Map.h"
#include "FrameLib_SampleRate.h"

// Parameters

#include "FrameLib_CombineTags.h"
#include "FrameLib_FilterTags.h"
#include "FrameLib_GetParam.h"
#include "FrameLib_Tag.h"
#include "FrameLib_Untag.h"

// Routing

#include "FrameLib_Dispatch.h"
#include "FrameLib_Route.h"
#include "FrameLib_Select.h"

// Schedulers

#include "FrameLib_AudioTrigger.h"
#include "FrameLib_Future.h"
#include "FrameLib_Interval.h"
#include "FrameLib_Once.h"
#include "FrameLib_PerBlock.h"

// Spatial

#include "FrameLib_CoordinateSystem.h"
#include "FrameLib_Spatial.h"

// Spectral

#include "FrameLib_FFT.h"
#include "FrameLib_iFFT.h"
#include "FrameLib_Correlate.h"
#include "FrameLib_Convolve.h"
#include "FrameLib_Multitaper.h"
#include "FrameLib_Window.h"

// Storage

#include "FrameLib_Recall.h"
#include "FrameLib_Register.h"
#include "FrameLib_Store.h"

// Streaming

#include "FrameLib_StreamID.h"

// Time Smoothing

#include "FrameLib_EWMA.h"
#include "FrameLib_EWMSD.h"
#include "FrameLib_FrameDelta.h"
#include "FrameLib_Lag.h"
#include "FrameLib_TimeMean.h"
#include "FrameLib_TimeMedian.h"
#include "FrameLib_TimeStdDev.h"

// Timing

#include "FrameLib_Now.h"
#include "FrameLib_Ticks.h"
#include "FrameLib_TimeDelta.h"
#include "FrameLib_Timer.h"

// Vector

#include "FrameLib_AccumPoint.h"
#include "FrameLib_Chop.h"
#include "FrameLib_Join.h"
#include "FrameLib_MedianFilter.h"
#include "FrameLib_NonZero.h"
#include "FrameLib_Pad.h"
#include "FrameLib_Peaks.h"
#include "FrameLib_Percentile.h"
#include "FrameLib_Shift.h"
#include "FrameLib_Sort.h"
#include "FrameLib_Split.h"
#include "FrameLib_Subframe.h"
#include "FrameLib_Vector_Objects.h"

// Operators

#include "FrameLib_Unary_Objects.h"
#include "FrameLib_Binary_Objects.h"
#include "FrameLib_Ternary_Objects.h"

// Complex Operators

#include "FrameLib_Complex_Unary_Objects.h"
#include "FrameLib_Cartopol.h"
#include "FrameLib_Poltocar.h"
#include "FrameLib_Complex_Binary_Objects.h"

#endif /* FRAMELIB_OBJECTS_H */
