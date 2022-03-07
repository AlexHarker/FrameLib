
#include "FrameLib_MaxClass.h"
#include "../FrameLib_Exports/FrameLib_Objects.h"

// Buffer

#include "../Buffer/fl.info~.cpp"
#include "../Buffer/fl.read~.cpp"

// Expression

#include "../Expressions/fl.expr~.cpp"
#include "../Expressions/fl.complex.expr~.cpp"

// Host Communication

#include "../Host_Communication/fl.tomax~.cpp"
#include "../Host_Communication/fl.frommax~.cpp"

// Context

#include "../Context/fl.contextcontrol~.cpp"

struct FrameLib_Dummy : public MaxClass_Base
{
    FrameLib_Dummy(t_object *x, t_symbol *sym, long ac, t_atom *av)
    {
        post("FrameLib Loaded");
    }
};

// Main setup routine

extern "C" int C74_EXPORT main(void)
{
    FrameLib_Dummy::makeClass<FrameLib_Dummy>(CLASS_BOX, "maxframelib");
    
    // Filters
    
    FrameLib_MaxClass_Expand<FrameLib_Biquad>::makeClass("fl.biquad~");
    FrameLib_MaxClass_Expand<FrameLib_BiquadCoeff>::makeClass("fl.biquadcoeff~");
    FrameLib_MaxClass_Expand<FrameLib_OnePole>::makeClass("fl.onepole~");
    FrameLib_MaxClass_Expand<FrameLib_OnePoleZero>::makeClass("fl.onepolezero~");
    FrameLib_MaxClass_Expand<FrameLib_SVF>::makeClass("fl.svf~");

    // Generators
    
    FrameLib_MaxClass_Expand<FrameLib_Gaussian>::makeClass("fl.gaussian~");
    FrameLib_MaxClass_Expand<FrameLib_MakeWindow>::makeClass("fl.makewindow~");
    FrameLib_MaxClass_Expand<FrameLib_Ramp>::makeClass("fl.ramp~");
    FrameLib_MaxClass_Expand<FrameLib_Random>::makeClass("fl.random~");
    FrameLib_MaxClass_Expand<FrameLib_Uniform>::makeClass("fl.uniform~");

    // IO
    
    FrameLib_MaxClass_Expand<FrameLib_Sink>::makeClass("fl.sink~");
    FrameLib_MaxClass_Expand<FrameLib_Source>::makeClass("fl.source~");
    FrameLib_MaxClass_Expand<FrameLib_Trace>::makeClass("fl.trace~");

    // Mapping
    
    FrameLib_MaxClass_Expand<FrameLib_Constant>::makeClass("fl.constant~");
    FrameLib_MaxClass_Expand<FrameLib_Convert>::makeClass("fl.convert~");
    FrameLib_MaxClass_Expand<FrameLib_Lookup>::makeClass("fl.lookup~");
    FrameLib_MaxClass_Expand<FrameLib_Map>::makeClass("fl.map~");
    FrameLib_MaxClass_Expand<FrameLib_SampleRate>::makeClass("fl.samplerate~");
    
    // Parameters
    
    FrameLib_MaxClass_Expand<FrameLib_CombineTags>::makeClass("fl.combinetags~");
    FrameLib_MaxClass_Expand<FrameLib_FilterTags>::makeClass("fl.filtertags~");
    FrameLib_MaxClass_Expand<FrameLib_LookupString>::makeClass("fl.lookupstring~");
    FrameLib_MaxClass_Expand<FrameLib_MakeString>::makeClass("fl.makestring~");
    FrameLib_MaxClass_Expand<FrameLib_Tag>::makeClass("fl.tag~");
    FrameLib_MaxClass_Expand<FrameLib_Untag>::makeClass("fl.untag~");
    
    // Routing
    
    FrameLib_MaxClass_Expand<FrameLib_Dispatch>::makeClass("fl.dispatch~");
    FrameLib_MaxClass_Expand<FrameLib_Route>::makeClass("fl.route~");
    FrameLib_MaxClass_Expand<FrameLib_Select>::makeClass("fl.select~");
    
    // Schedulers
    
    FrameLib_MaxClass_Expand<FrameLib_AudioTrigger>::makeClass("fl.audiotrigger~");
    FrameLib_MaxClass_Expand<FrameLib_Chain>::makeClass("fl.chain~");
    FrameLib_MaxClass_Expand<FrameLib_Interval>::makeClass("fl.interval~");
    FrameLib_MaxClass_Expand<FrameLib_Once>::makeClass("fl.once~");
    FrameLib_MaxClass_Expand<FrameLib_PerBlock>::makeClass("fl.perblock~");
    
    // Spatial
    
    FrameLib_MaxClass_Expand<FrameLib_Cartopol>::makeClass("fl.cartopol~");
    FrameLib_MaxClass_Expand<FrameLib_Poltocar>::makeClass("fl.poltocar~");
    FrameLib_MaxClass_Expand<FrameLib_Spatial>::makeClass("fl.spatial~");
    
    // Spectral
    
    FrameLib_MaxClass_Expand<FrameLib_FFT>::makeClass("fl.fft~");
    FrameLib_MaxClass_Expand<FrameLib_iFFT>::makeClass("fl.ifft~");
    FrameLib_MaxClass_Expand<FrameLib_Correlate>::makeClass("fl.correlate~");
    FrameLib_MaxClass_Expand<FrameLib_Convolve>::makeClass("fl.convolve~");
    FrameLib_MaxClass_Expand<FrameLib_Multitaper>::makeClass("fl.multitaper~");
    FrameLib_MaxClass_Expand<FrameLib_Window>::makeClass("fl.window~");
    
    // Storage
    
    FrameLib_MaxClass_Expand<FrameLib_Recall>::makeClass("fl.recall~");
    FrameLib_MaxClass_Expand<FrameLib_Register, kAllInputs>::makeClass("fl.register~");
    FrameLib_MaxClass_Expand<FrameLib_Store>::makeClass("fl.store~");
    
    // Streaming
    
    FrameLib_MaxClass<FrameLib_Unpack>::makeClass("fl.unpack~");
    FrameLib_MaxClass<FrameLib_Pack>::makeClass("fl.pack~");
    FrameLib_MaxClass_Expand<FrameLib_StreamID>::makeClass("fl.streamid~");
    
    // Time Smoothing
    
    FrameLib_MaxClass_Expand<FrameLib_FrameDelta>::makeClass("fl.framedelta~");
    FrameLib_MaxClass_Expand<FrameLib_Lag>::makeClass("fl.lag~");
    FrameLib_MaxClass_Expand<FrameLib_MovingAverage>::makeClass("fl.movingaverage~");
    FrameLib_MaxClass_Expand<FrameLib_TimeMean>::makeClass("fl.timemean~");
    FrameLib_MaxClass_Expand<FrameLib_TimeMedian>::makeClass("fl.timemedian~");
    FrameLib_MaxClass_Expand<FrameLib_TimeStdDev>::makeClass("fl.timestddev~");
    
    // Timing
    
    FrameLib_MaxClass_Expand<FrameLib_Now>::makeClass("fl.now~");
    FrameLib_MaxClass_Expand<FrameLib_Ticks>::makeClass("fl.ticks~");
    FrameLib_MaxClass_Expand<FrameLib_TimeDelta>::makeClass("fl.timedelta~");
    FrameLib_MaxClass_Expand<FrameLib_Timer>::makeClass("fl.timer~");
    
    // Vector
    
    FrameLib_MaxClass_Expand<FrameLib_AccumPos>::makeClass("fl.accumpos~");
    FrameLib_MaxClass_Expand<FrameLib_Chop>::makeClass("fl.chop~");
    FrameLib_MaxClass_Expand<FrameLib_Join>::makeClass("fl.join~");
    FrameLib_MaxClass_Expand<FrameLib_MedianFilter>::makeClass("fl.medianfilter~");
    FrameLib_MaxClass_Expand<FrameLib_NonZero>::makeClass("fl.nonzero~");
    FrameLib_MaxClass_Expand<FrameLib_Pad>::makeClass("fl.pad~");
    FrameLib_MaxClass_Expand<FrameLib_Peaks>::makeClass("fl.pattern~");
    FrameLib_MaxClass_Expand<FrameLib_Peaks>::makeClass("fl.peaks~");
    FrameLib_MaxClass_Expand<FrameLib_Percentile>::makeClass("fl.percentile~");
    FrameLib_MaxClass_Expand<FrameLib_Prioritise>::makeClass("fl.prioritise~");
    FrameLib_MaxClass_Expand<FrameLib_Reverse>::makeClass("fl.reverse~");
    FrameLib_MaxClass_Expand<FrameLib_Shift>::makeClass("fl.shift~");
    FrameLib_MaxClass_Expand<FrameLib_Sort>::makeClass("fl.sort~");
    FrameLib_MaxClass_Expand<FrameLib_SortMultiple>::makeClass("fl.sortmultiple~");
    FrameLib_MaxClass_Expand<FrameLib_Split>::makeClass("fl.split~");
    FrameLib_MaxClass_Expand<FrameLib_Subframe>::makeClass("fl.subframe~");

    FrameLib_MaxClass_Expand<FrameLib_Length>::makeClass("fl.length~");
    FrameLib_MaxClass_Expand<FrameLib_FrameMin>::makeClass("fl.framemin~");
    FrameLib_MaxClass_Expand<FrameLib_FrameMax>::makeClass("fl.framemax~");
    FrameLib_MaxClass_Expand<FrameLib_Sum>::makeClass("fl.sum~");
    FrameLib_MaxClass_Expand<FrameLib_Product>::makeClass("fl.product~");
    FrameLib_MaxClass_Expand<FrameLib_Mean>::makeClass("fl.mean~");
    FrameLib_MaxClass_Expand<FrameLib_GeometricMean>::makeClass("fl.geomean~");
    FrameLib_MaxClass_Expand<FrameLib_StandardDeviation>::makeClass("fl.stddev~");

    FrameLib_MaxClass_Expand<FrameLib_Centroid>::makeClass("fl.centroid~");
    FrameLib_MaxClass_Expand<FrameLib_Spread>::makeClass("fl.spread~");
    FrameLib_MaxClass_Expand<FrameLib_Skewness>::makeClass("fl.skewness~");
    FrameLib_MaxClass_Expand<FrameLib_Kurtosis>::makeClass("fl.kurtosis~");
    FrameLib_MaxClass_Expand<FrameLib_Flatness>::makeClass("fl.flatness~");
    FrameLib_MaxClass_Expand<FrameLib_RMS>::makeClass("fl.rms~");
    FrameLib_MaxClass_Expand<FrameLib_Crest>::makeClass("fl.crest~");
    FrameLib_MaxClass_Expand<FrameLib_FrameMinPosition>::makeClass("fl.minpos~");
    FrameLib_MaxClass_Expand<FrameLib_FrameMaxPosition>::makeClass("fl.maxpos~");
    FrameLib_MaxClass_Expand<FrameLib_NanFilter>::makeClass("fl.nanfilter~");

    // Unary Operators
    
    FrameLib_MaxClass_Expand<FrameLib_LogicalNot>::makeClass("fl.not~");
    
    FrameLib_MaxClass_Expand<FrameLib_Acos>::makeClass("fl.acos~");
    FrameLib_MaxClass_Expand<FrameLib_Asin>::makeClass("fl.asin~");
    FrameLib_MaxClass_Expand<FrameLib_Atan>::makeClass("fl.atan~");
    FrameLib_MaxClass_Expand<FrameLib_Cos>::makeClass("fl.cos~");
    FrameLib_MaxClass_Expand<FrameLib_Sin>::makeClass("fl.sin~");
    FrameLib_MaxClass_Expand<FrameLib_Tan>::makeClass("fl.tan~");
    FrameLib_MaxClass_Expand<FrameLib_Acosh>::makeClass("fl.acosh~");
    FrameLib_MaxClass_Expand<FrameLib_Asinh>::makeClass("fl.asinh~");
    FrameLib_MaxClass_Expand<FrameLib_Atanh>::makeClass("fl.atanh~");
    FrameLib_MaxClass_Expand<FrameLib_Cosh>::makeClass("fl.cosh~");
    FrameLib_MaxClass_Expand<FrameLib_Sinh>::makeClass("fl.sinh~");
    FrameLib_MaxClass_Expand<FrameLib_Tanh>::makeClass("fl.tanh~");
    
    FrameLib_MaxClass_Expand<FrameLib_Log>::makeClass("fl.log~");
    FrameLib_MaxClass_Expand<FrameLib_Log2>::makeClass("fl.log2~");
    FrameLib_MaxClass_Expand<FrameLib_Log10>::makeClass("fl.log10~");
    FrameLib_MaxClass_Expand<FrameLib_Exp>::makeClass("fl.exp~");
    FrameLib_MaxClass_Expand<FrameLib_Exp2>::makeClass("fl.exp2~");
    
    FrameLib_MaxClass_Expand<FrameLib_Abs>::makeClass("fl.abs~");
    FrameLib_MaxClass_Expand<FrameLib_Ceil>::makeClass("fl.ceil~");
    FrameLib_MaxClass_Expand<FrameLib_Floor>::makeClass("fl.floor~");
    FrameLib_MaxClass_Expand<FrameLib_Round>::makeClass("fl.round~");
    FrameLib_MaxClass_Expand<FrameLib_Trunc>::makeClass("fl.trunc~");
    
    FrameLib_MaxClass_Expand<FrameLib_Sqrt>::makeClass("fl.sqrt~");
    FrameLib_MaxClass_Expand<FrameLib_Cbrt>::makeClass("fl.cbrt~");
    FrameLib_MaxClass_Expand<FrameLib_Erf>::makeClass("fl.erf~");
    FrameLib_MaxClass_Expand<FrameLib_Erfc>::makeClass("fl.erfc~");
    
    // Binary Operators
    
    FrameLib_MaxClass_Expand<FrameLib_Add, kAllInputs>::makeClass("fl.plus~");
    FrameLib_MaxClass_Expand<FrameLib_Subtract, kAllInputs>::makeClass("fl.minus~");
    FrameLib_MaxClass_Expand<FrameLib_Multiply, kAllInputs>::makeClass("fl.times~");
    FrameLib_MaxClass_Expand<FrameLib_Divide, kAllInputs>::makeClass("fl.divide~");
    
    FrameLib_MaxClass_Expand<FrameLib_Equal, kAllInputs>::makeClass("fl.equals~");
    FrameLib_MaxClass_Expand<FrameLib_NotEqual, kAllInputs>::makeClass("fl.notequals~");
    FrameLib_MaxClass_Expand<FrameLib_GreaterThan, kAllInputs>::makeClass("fl.greaterthan~");
    FrameLib_MaxClass_Expand<FrameLib_LessThan, kAllInputs>::makeClass("fl.lessthan~");
    FrameLib_MaxClass_Expand<FrameLib_GreaterThanEqual, kAllInputs>::makeClass("fl.fl.greaterthaneq~");
    FrameLib_MaxClass_Expand<FrameLib_LessThanEqual, kAllInputs>::makeClass("fl.lessthaneq~");
    FrameLib_MaxClass_Expand<FrameLib_LogicalAnd, kAllInputs>::makeClass("fl.and~");
    FrameLib_MaxClass_Expand<FrameLib_LogicalOr, kAllInputs>::makeClass("fl.or~");
    
    FrameLib_MaxClass_Expand<FrameLib_Pow, kAllInputs>::makeClass("fl.pow~");
    FrameLib_MaxClass_Expand<FrameLib_Atan2, kAllInputs>::makeClass("fl.atan2~");
    FrameLib_MaxClass_Expand<FrameLib_Hypot, kAllInputs>::makeClass("fl.hypot~");
    FrameLib_MaxClass_Expand<FrameLib_CopySign, kAllInputs>::makeClass("fl.copysign~");
    FrameLib_MaxClass_Expand<FrameLib_Min, kAllInputs>::makeClass("fl.min~");
    FrameLib_MaxClass_Expand<FrameLib_Max, kAllInputs>::makeClass("fl.max~");
    FrameLib_MaxClass_Expand<FrameLib_Diff, kAllInputs>::makeClass("fl.diff~");
    FrameLib_MaxClass_Expand<FrameLib_Modulo, kAllInputs>::makeClass("fl.modulo~");

    // Ternary  Operators
    
    FrameLib_MaxClass_Expand<FrameLib_Clip, kDistribute>::makeClass("fl.clip~");
    FrameLib_MaxClass_Expand<FrameLib_Fold, kDistribute>::makeClass("fl.fold~");
    FrameLib_MaxClass_Expand<FrameLib_Wrap, kDistribute>::makeClass("fl.wrap~");
    
    // Complex Unary Operators
    
    FrameLib_MaxClass_Expand<FrameLib_Complex_Cos>::makeClass("fl.complex.cos~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Sin>::makeClass("fl.complex.sin~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Tan>::makeClass("fl.complex.tan~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Cosh>::makeClass("fl.complex.cosh~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Sinh>::makeClass("fl.complex.sinh~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Tanh>::makeClass("fl.complex.tanh~");

    FrameLib_MaxClass_Expand<FrameLib_Complex_Log>::makeClass("fl.complex.log~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Log10>::makeClass("fl.complex.log10~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Exp>::makeClass("fl.complex.exp~");

    FrameLib_MaxClass_Expand<FrameLib_Complex_Sqrt>::makeClass("fl.complex.sqrt~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Conj>::makeClass("fl.complex.conj~");
    
    // Complex Binary Operators
    
    FrameLib_MaxClass_Expand<FrameLib_Complex_Add, kAllInputs>::makeClass("fl.complex.plus~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Subtract, kAllInputs>::makeClass("fl.complex.minus~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Multiply, kAllInputs>::makeClass("fl.complex.times~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Divide, kAllInputs>::makeClass("fl.complex.divide~");
    FrameLib_MaxClass_Expand<FrameLib_Complex_Pow, kAllInputs>::makeClass("fl.complex.pow~");

    // Buffer
    
    FrameLib_MaxClass_Info::makeClass<FrameLib_MaxClass_Info>("fl.info~");
    FrameLib_MaxClass_Read::makeClass<FrameLib_MaxClass_Read>("fl.read~");
    
    // Expressions
    
    FrameLib_MaxClass_Expression::makeClass<FrameLib_MaxClass_Expression>("fl.expr~");
    FrameLib_MaxClass_ComplexExpression::makeClass<FrameLib_MaxClass_ComplexExpression>("fl.complex.expr~");
    
    // Host Communication
    
    FrameLib_MaxClass_ToMax::makeClass<FrameLib_MaxClass_ToMax>("fl.tomax~");
    FrameLib_MaxClass_FromMax::makeClass<FrameLib_MaxClass_FromMax>("fl.frommax~");
    
    // Context
    
    FrameLib_MaxClass_ContextControl::makeClass<FrameLib_MaxClass_ContextControl>(CLASS_BOX, "fl.contextcontrol~");
}
