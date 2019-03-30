// fl includes
#include "FrameLib_Objects.h"
#include "FrameLib_Global.h"
#include "FrameLib_Context.h"
#include "FrameLib_Multistream.h"
// stl
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

void write_info()
    {
        std::vector<std::string> objectNames = {};
        objectNames.resize(160);
        objectNames[0] = "fl.dispatch~";
        objectNames[1] = "fl.now~";
        objectNames[2] = "fl.complextan~";
        objectNames[3] = "fl.route~";
        objectNames[4] = "fl.mean~";
        objectNames[5] = "fl.sort~";
        objectNames[6] = "fl.cos~";
        objectNames[7] = "fl.read~";
        objectNames[8] = "fl.samplerate~";
        objectNames[9] = "fl.ewmsd~";
//        objectNames[10] = "fl.complexsin~";
//        objectNames[11] = "fl.join~";
//        objectNames[12] = "fl.equals~";
//        objectNames[13] = "fl.fft~";
//        objectNames[14] = "fl.tomax~";
//        objectNames[15] = "fl.divide~";
//        objectNames[16] = "fl.atan~";
//        objectNames[17] = "fl.filtertags~";
//        objectNames[18] = "fl.timemean~";
//        objectNames[19] = "fl.cosh~";
//        objectNames[20] = "fl.asin~";
//        objectNames[21] = "fl.complexdivide~";
//        objectNames[22] = "fl.or~";
//        objectNames[23] = "fl.nonzero~";
//        objectNames[24] = "fl.vmax~";
//        objectNames[25] = "fl.vmin~";
//        objectNames[26] = "fl.map~";
//        objectNames[27] = "fl.complexplus~";
//        objectNames[28] = "Store";
//        objectNames[29] = "fl.subframe~";
//        objectNames[30] = "fl.audiotrigger~";
//        objectNames[31] = "fl.chain~";
//        objectNames[32] = "fl.convert~";
//        objectNames[33] = "fl.random~";
//        objectNames[34] = "fl.ticks~";
//        objectNames[35] = "fl.correlate~";
//        objectNames[36] = "fl.0dfsvf~";
//        objectNames[37] = "fl.sallenkey~";
//        objectNames[38] = "fl.peaks~";
//        objectNames[39] = "fl.complexconj~";
//        objectNames[40] = "fl.info~";
//        objectNames[41] = "fl.sum~";
//        objectNames[42] = "fl.trunc~";
//        objectNames[43] = "fl.asinh~";
//        objectNames[44] = "fl.getparam~";
//        objectNames[45] = "fl.copysign~";
//        objectNames[46] = "fl.greaterthaneq~";
//        objectNames[47] = "fl.framedelta~";
//        objectNames[48] = "fl.argmax~";
//        objectNames[49] = "fl.complexsinh~";
//        objectNames[50] = "fl.skewness~";
//        objectNames[51] = "fl.exp~";
//        objectNames[52] = "fl.pow~";
//        objectNames[53] = "fl.spread~";
//        objectNames[54] = "fl.sqrt~";
//        objectNames[55] = "fl.nanfilter~";
//        objectNames[56] = "fl.crest~";
//        objectNames[57] = "fl.complexlog~";
//        objectNames[58] = "fl.flatness~";
//        objectNames[59] = "fl.cbrt~";
//        objectNames[60] = "fl.tanh~";
//        objectNames[61] = "fl.sink~";
//        objectNames[62] = "fl.notequals~";
//        objectNames[63] = "fl.split~";
//        objectNames[64] = "fl.wrap~";
//        objectNames[65] = "fl.argmin~";
//        objectNames[66] = "fl.ewma~";
//        objectNames[67] = "fl.round~";
//        objectNames[68] = "fl.exp2~";
//        objectNames[69] = "fl.erf~";
//        objectNames[70] = "fl.lessthaneq~";
//        objectNames[71] = "fl.complexlog10~";
//        objectNames[72] = "fl.tag~";
//        objectNames[73] = "fl.streamid~";
//        objectNames[74] = "fl.source~";
//        objectNames[75] = "fl.complexexpr~";
//        objectNames[76] = "fl.lag~";
//        objectNames[77] = "fl.poltocar~";
//        objectNames[78] = "fl.timemedian~";
//        objectNames[79] = "fl.ceil~";
//        objectNames[80] = "fl.floor~";
//        objectNames[81] = "fl.lessthan~";
//        objectNames[82] = "fl.uniform~";
//        objectNames[83] = "fl.chop~";
//        objectNames[84] = "fl.reverse~";
//        objectNames[85] = "fl.coordinatesystem~";
//        objectNames[86] = "fl.atan2~";
//        objectNames[87] = "fl.lookup~";
//        objectNames[88] = "fl.store~";
//        objectNames[89] = "fl.complexexp~";
//        objectNames[90] = "fl.complexpow~";
//        objectNames[91] = "fl.sinh~";
//        objectNames[92] = "fl.complexsqrt~";
//        objectNames[93] = "fl.min~";
//        objectNames[94] = "fl.standarddeviation~";
//        objectNames[95] = "fl.window~";
//        objectNames[96] = "fl.log~";
//        objectNames[97] = "fl.minus~";
//        objectNames[98] = "fl.abs~";
//        objectNames[99] = "fl.geometricmean~";
//        objectNames[100] = "fl.and~";
//        objectNames[101] = "fl.complextanh~";
//        objectNames[102] = "fl.ramp~";
//        objectNames[103] = "fl.resonant~";
//        objectNames[104] = "fl.centroid~";
//        objectNames[105] = "fl.max~";
//        objectNames[106] = "fl.onepolezero~";
//        objectNames[107] = "fl.atanh~";
//        objectNames[108] = "fl.unpack~";
//        objectNames[109] = "fl.multitaper~";
//        objectNames[110] = "fl.trace~";
//        objectNames[111] = "fl.expr~";
//        objectNames[112] = "fl.recall~";
//        objectNames[113] = "fl.medianfilter~";
//        objectNames[114] = "fl.select~";
//        objectNames[115] = "fl.interval~";
//        objectNames[116] = "fl.future~";
//        objectNames[117] = "fl.modulo~";
//        objectNames[118] = "fl.sin~";
//        objectNames[119] = "fl.percentile~";
//        objectNames[120] = "fl.once~";
//        objectNames[121] = "fl.cartopol~";
//        objectNames[122] = "fl.acosh~";
//        objectNames[123] = "fl.complexcos~";
//        objectNames[124] = "fl.fold~";
//        objectNames[125] = "fl.tan~";
//        objectNames[126] = "fl.timestddev~";
//        objectNames[127] = "fl.frommax~";
//        objectNames[128] = "fl.kurtosis~";
//        objectNames[129] = "fl.timedelta~";
//        objectNames[130] = "fl.clip~";
//        objectNames[131] = "fl.acos~";
//        objectNames[132] = "fl.complexcosh~";
//        objectNames[133] = "fl.length~";
//        objectNames[134] = "fl.complexmultiply~";
//        objectNames[135] = "fl.timer~";
//        objectNames[136] = "fl.onepole~";
//        objectNames[137] = "fl.erfc~";
//        objectNames[138] = "fl.hypot~";
//        objectNames[139] = "fl.greaterthan~";
//        objectNames[140] = "fl.rms~";
//        objectNames[141] = "fl.log10~";
//        objectNames[142] = "fl.plus~";
//        objectNames[143] = "fl.times~";
//        objectNames[144] = "fl.constant~";
//        objectNames[145] = "fl.untag~";
//        objectNames[146] = "fl.perblock~";
//        objectNames[147] = "fl.gaussian~";
//        objectNames[148] = "fl.pad~";
//        objectNames[149] = "fl.diff~";
//        objectNames[150] = "fl.register~";
//        objectNames[151] = "fl.not~";
//        objectNames[152] = "fl.spatial~";
//        objectNames[153] = "fl.combinetags~";
//        objectNames[154] = "fl.pack~";
//        objectNames[155] = "fl.accumpoint~";
//        objectNames[156] = "fl.product~";
//        objectNames[157] = "fl.ifft~";
//        objectNames[158] = "fl.shift~";
//        objectNames[159] = "fl.convolve~";

    
        // Create a Global, Context
        FrameLib_Global *Global = nullptr;
        FrameLib_Global::get(&Global);
        std::vector<FrameLib_Multistream *> cpp_objects;
        cpp_objects.resize(160);
        FrameLib_Proxy *Proxy = new FrameLib_Proxy();
        FrameLib_Context context(Global, nullptr);
        FrameLib_Parameters::AutoSerial parameters;
        
        cpp_objects[0] = new FrameLib_Expand<FrameLib_Dispatch>(context, &parameters, Proxy, 1);
        cpp_objects[1] = new FrameLib_Expand<FrameLib_Now>(context, &parameters, Proxy, 1);
        cpp_objects[2] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::__1::tan<double>> > >(context, &parameters, Proxy, 1);
        cpp_objects[3] = new FrameLib_Expand<FrameLib_Route>(context, &parameters, Proxy, 1);
        cpp_objects[4] = new FrameLib_Expand<FrameLib_Vector<&statMean<double const*>, false> >(context, &parameters, Proxy, 1);
        cpp_objects[5] = new FrameLib_Expand<FrameLib_Sort>(context, &parameters, Proxy, 1);
        cpp_objects[6] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&cos> > >(context, &parameters, Proxy, 1);
        cpp_objects[7] = new FrameLib_Expand<FrameLib_Read>(context, &parameters, Proxy, 1);
        cpp_objects[8] = new FrameLib_Expand<FrameLib_SampleRate>(context, &parameters, Proxy, 1);
        cpp_objects[9] = new FrameLib_Expand<FrameLib_EWMSD>(context, nullptr, Proxy, 1);
//        cpp_objects[10] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::__1::sin<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[11] = new FrameLib_Expand<FrameLib_Join>(context, &parameters, Proxy, 1);
//        cpp_objects[12] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::equal_to<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[13] = new FrameLib_Expand<FrameLib_FFT>(context, &parameters, Proxy, 1);
//        cpp_objects[14] = new FrameLib_Expand<FrameLib_ToHost>(context, &parameters, Proxy, 1);
//        cpp_objects[15] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::divides<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[16] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&atan> > >(context, &parameters, Proxy, 1);
//        cpp_objects[17] = new FrameLib_Expand<FrameLib_FilterTags>(context, &parameters, Proxy, 1);
//        cpp_objects[18] = new FrameLib_Expand<FrameLib_TimeMean>(context, &parameters, Proxy, 1);
//        cpp_objects[19] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&cosh> > >(context, &parameters, Proxy, 1);
//        cpp_objects[20] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&asin> > >(context, &parameters, Proxy, 1);
//        cpp_objects[21] = new FrameLib_Expand<FrameLib_Complex_BinaryOp<std::__1::divides<std::__1::complex<double> > > >(context, &parameters, Proxy, 1);
//        cpp_objects[22] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::logical_or<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[23] = new FrameLib_Expand<FrameLib_NonZero>(context, &parameters, Proxy, 1);
//        cpp_objects[24] = new FrameLib_Expand<FrameLib_Vector<&statMax<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[25] = new FrameLib_Expand<FrameLib_Vector<&statMin<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[26] = new FrameLib_Expand<FrameLib_Map>(context, &parameters, Proxy, 1);
//        cpp_objects[27] = new FrameLib_Expand<FrameLib_Complex_BinaryOp<std::__1::plus<std::__1::complex<double> > > >(context, &parameters, Proxy, 1);
//        cpp_objects[29] = new FrameLib_Expand<FrameLib_Subframe>(context, &parameters, Proxy, 1);
//        cpp_objects[30] = new FrameLib_Expand<FrameLib_AudioTrigger>(context, &parameters, Proxy, 1);
//        cpp_objects[31] = new FrameLib_Expand<FrameLib_Chain>(context, &parameters, Proxy, 1);
//        cpp_objects[32] = new FrameLib_Expand<FrameLib_Convert>(context, &parameters, Proxy, 1);
//        cpp_objects[33] = new FrameLib_Expand<FrameLib_Random>(context, &parameters, Proxy, 1);
//        cpp_objects[34] = new FrameLib_Expand<FrameLib_Ticks>(context, &parameters, Proxy, 1);
//        cpp_objects[35] = new FrameLib_Expand<FrameLib_Correlate>(context, &parameters, Proxy, 1);
//        cpp_objects[36] = new FrameLib_Expand<FrameLib_0dfSVF>(context, &parameters, Proxy, 1);
//        cpp_objects[37] = new FrameLib_Expand<FrameLib_SallenAndKey>(context, &parameters, Proxy, 1);
//        cpp_objects[38] = new FrameLib_Expand<FrameLib_Peaks>(context, &parameters, Proxy, 1);
//        cpp_objects[39] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::__1::conj<double>> > >(context, &parameters, Proxy, 1);
////        cpp_objects[40] = new FrameLib_Expand<FrameLib_Info>(context, &parameters, Proxy, 1);
//        cpp_objects[40] = new FrameLib_Expand<FrameLib_Peaks>(context, &parameters, Proxy, 1);
//        cpp_objects[41] = new FrameLib_Expand<FrameLib_Vector<&statSum<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[42] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&trunc> > >(context, &parameters, Proxy, 1);
//        cpp_objects[43] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&asinh> > >(context, &parameters, Proxy, 1);
//        cpp_objects[44] = new FrameLib_Expand<FrameLib_GetParam>(context, &parameters, Proxy, 1);
//        cpp_objects[45] = new FrameLib_Expand<FrameLib_BinaryOp<Binary_Functor<&copysign> > >(context, &parameters, Proxy, 1);
//        cpp_objects[46] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::greater_equal<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[47] = new FrameLib_Expand<FrameLib_FrameDelta>(context, &parameters, Proxy, 1);
//        cpp_objects[48] = new FrameLib_Expand<FrameLib_Vector<&statArgMax<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[49] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::__1::sinh<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[50] = new FrameLib_Expand<FrameLib_Vector<&statSkewness<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[51] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&exp> > >(context, &parameters, Proxy, 1);
//        cpp_objects[52] = new FrameLib_Expand<FrameLib_BinaryOp<Binary_Functor<&pow> > >(context, &parameters, Proxy, 1);
//        cpp_objects[53] = new FrameLib_Expand<FrameLib_Vector<&statSpread<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[54] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&sqrt> > >(context, &parameters, Proxy, 1);
//        cpp_objects[55] = new FrameLib_Expand<FrameLib_NanFilter>(context, &parameters, Proxy, 1);
//        cpp_objects[56] = new FrameLib_Expand<FrameLib_Vector<&statCrest<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[57] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::__1::log<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[58] = new FrameLib_Expand<FrameLib_Vector<&statFlatness<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[59] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&cbrt> > >(context, &parameters, Proxy, 1);
//        cpp_objects[60] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&tanh> > >(context, &parameters, Proxy, 1);
//        cpp_objects[61] = new FrameLib_Expand<FrameLib_Sink>(context, &parameters, Proxy, 1);
//        cpp_objects[62] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::not_equal_to<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[63] = new FrameLib_Expand<FrameLib_Split>(context, &parameters, Proxy, 1);
//        cpp_objects[64] = new FrameLib_Expand<FrameLib_TernaryOp<Ternary_Functor<&FrameLib_Ternary_Ops::wrap<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[65] = new FrameLib_Expand<FrameLib_Vector<&statArgMin<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[66] = new FrameLib_Expand<FrameLib_EWMA>(context, &parameters, Proxy, 1);
//        cpp_objects[67] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&round> > >(context, &parameters, Proxy, 1);
//        cpp_objects[68] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&exp2> > >(context, &parameters, Proxy, 1);
//        cpp_objects[69] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&erf> > >(context, &parameters, Proxy, 1);
//        cpp_objects[70] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::less_equal<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[71] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::__1::log10<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[72] = new FrameLib_Expand<FrameLib_Tag>(context, &parameters, Proxy, 1);
//        cpp_objects[73] = new FrameLib_Expand<FrameLib_StreamID>(context, &parameters, Proxy, 1);
//        cpp_objects[74] = new FrameLib_Expand<FrameLib_Source>(context, &parameters, Proxy, 1);
//        cpp_objects[75] = new FrameLib_Expand<FrameLib_ComplexExpression>(context, &parameters, Proxy, 1);
//        cpp_objects[76] = new FrameLib_Expand<FrameLib_Lag>(context, &parameters, Proxy, 1);
//        cpp_objects[77] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<PolToCar> >(context, &parameters, Proxy, 1);
//        cpp_objects[78] = new FrameLib_Expand<FrameLib_TimeMedian>(context, &parameters, Proxy, 1);
//        cpp_objects[79] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&ceil> > >(context, &parameters, Proxy, 1);
//        cpp_objects[80] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&floor> > >(context, &parameters, Proxy, 1);
//        cpp_objects[81] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::less<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[82] = new FrameLib_Expand<FrameLib_Uniform>(context, &parameters, Proxy, 1);
//        cpp_objects[83] = new FrameLib_Expand<FrameLib_Chop>(context, &parameters, Proxy, 1);
//        cpp_objects[84] = new FrameLib_Expand<FrameLib_Reverse>(context, &parameters, Proxy, 1);
//        cpp_objects[85] = new FrameLib_Expand<FrameLib_CoordinateSystem>(context, &parameters, Proxy, 1);
//        cpp_objects[86] = new FrameLib_Expand<FrameLib_BinaryOp<Binary_Functor<&atan2> > >(context, &parameters, Proxy, 1);
//        cpp_objects[87] = new FrameLib_Expand<FrameLib_Lookup>(context, &parameters, Proxy, 1);
//        cpp_objects[88] = new FrameLib_Expand<FrameLib_Store>(context, &parameters, Proxy, 1);
//        cpp_objects[89] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::__1::exp<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[90] = new FrameLib_Expand<FrameLib_Complex_BinaryOp<Complex_Binary_Functor<&std::__1::pow<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[91] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&sinh> > >(context, &parameters, Proxy, 1);
//        cpp_objects[92] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::__1::sqrt<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[93] = new FrameLib_Expand<FrameLib_BinaryOp<Binary_Functor<&fmin> > >(context, &parameters, Proxy, 1);
//        cpp_objects[94] = new FrameLib_Expand<FrameLib_Vector<&statStandardDeviation<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[95] = new FrameLib_Expand<FrameLib_Window>(context, &parameters, Proxy, 1);
//        cpp_objects[96] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&log> > >(context, &parameters, Proxy, 1);
//        cpp_objects[97] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::minus<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[98] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&fabs> > >(context, &parameters, Proxy, 1);
//        cpp_objects[99] = new FrameLib_Expand<FrameLib_Vector<&statGeometricMean<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[100] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::logical_and<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[101] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::__1::tanh<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[102] = new FrameLib_Expand<FrameLib_Ramp>(context, &parameters, Proxy, 1);
//        cpp_objects[103] = new FrameLib_Expand<FrameLib_Resonant>(context, &parameters, Proxy, 1);
//        cpp_objects[104] = new FrameLib_Expand<FrameLib_Vector<&statCentroid<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[105] = new FrameLib_Expand<FrameLib_BinaryOp<Binary_Functor<&fmax> > >(context, &parameters, Proxy, 1);
////        cpp_objects[106] = new FrameLib_Expand<FrameLib_OnePoleZero>(context, &parameters, Proxy, 1);
//        cpp_objects[105] = new FrameLib_Expand<FrameLib_BinaryOp<Binary_Functor<&fmax> > >(context, &parameters, Proxy, 1);
//        cpp_objects[107] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&atanh> > >(context, &parameters, Proxy, 1);
//        cpp_objects[108] = new FrameLib_Unpack(context, &parameters, Proxy, 1);
//        cpp_objects[109] = new FrameLib_Expand<FrameLib_Multitaper>(context, &parameters, Proxy, 1);
//        cpp_objects[110] = new FrameLib_Expand<FrameLib_Trace>(context, &parameters, Proxy, 1);
//        cpp_objects[111] = new FrameLib_Expand<FrameLib_Expression>(context, &parameters, Proxy, 1);
//        cpp_objects[112] = new FrameLib_Expand<FrameLib_Recall>(context, &parameters, Proxy, 1);
//        cpp_objects[113] = new FrameLib_Expand<FrameLib_MedianFilter>(context, &parameters, Proxy, 1);
//        cpp_objects[114] = new FrameLib_Expand<FrameLib_Select>(context, &parameters, Proxy, 1);
//        cpp_objects[115] = new FrameLib_Expand<FrameLib_Interval>(context, &parameters, Proxy, 1);
//        cpp_objects[116] = new FrameLib_Expand<FrameLib_Future>(context, &parameters, Proxy, 1);
//        cpp_objects[117] = new FrameLib_Expand<FrameLib_BinaryOp<Binary_Functor<&fmod> > >(context, &parameters, Proxy, 1);
//        cpp_objects[118] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&sin> > >(context, &parameters, Proxy, 1);
//        cpp_objects[119] = new FrameLib_Expand<FrameLib_Percentile>(context, &parameters, Proxy, 1);
//        cpp_objects[120] = new FrameLib_Expand<FrameLib_Once>(context, &parameters, Proxy, 1);
//        cpp_objects[121] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<CarToPol> >(context, &parameters, Proxy, 1);
//        cpp_objects[122] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&acosh> > >(context, &parameters, Proxy, 1);
//        cpp_objects[123] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::__1::cos<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[124] = new FrameLib_Expand<FrameLib_TernaryOp<Ternary_Functor<&FrameLib_Ternary_Ops::fold<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[125] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&tan> > >(context, &parameters, Proxy, 1);
//        cpp_objects[126] = new FrameLib_Expand<FrameLib_TimeStdDev>(context, &parameters, Proxy, 1);
//        cpp_objects[127] = new FrameLib_Expand<FrameLib_FromHost>(context, &parameters, Proxy, 1);
//        cpp_objects[128] = new FrameLib_Expand<FrameLib_Vector<&statKurtosis<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[129] = new FrameLib_Expand<FrameLib_TimeDelta>(context, &parameters, Proxy, 1);
//        cpp_objects[130] = new FrameLib_Expand<FrameLib_TernaryOp<Ternary_Functor<&FrameLib_Ternary_Ops::clip<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[131] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&acos> > >(context, &parameters, Proxy, 1);
//        cpp_objects[132] = new FrameLib_Expand<FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::__1::cosh<double>> > >(context, &parameters, Proxy, 1);
//        cpp_objects[133] = new FrameLib_Expand<FrameLib_Vector<&statLength<double const*>, true> >(context, &parameters, Proxy, 1);
//        cpp_objects[134] = new FrameLib_Expand<FrameLib_Complex_BinaryOp<std::__1::multiplies<std::__1::complex<double> > > >(context, &parameters, Proxy, 1);
//        cpp_objects[135] = new FrameLib_Expand<FrameLib_Timer>(context, &parameters, Proxy, 1);
//        cpp_objects[136] = new FrameLib_Expand<FrameLib_OnePole>(context, &parameters, Proxy, 1);
//        cpp_objects[137] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&erfc> > >(context, &parameters, Proxy, 1);
//        cpp_objects[138] = new FrameLib_Expand<FrameLib_BinaryOp<Binary_Functor<&hypot> > >(context, &parameters, Proxy, 1);
//        cpp_objects[139] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::greater<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[140] = new FrameLib_Expand<FrameLib_Vector<&statRMS<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[141] = new FrameLib_Expand<FrameLib_UnaryOp<Unary_Functor<&log10> > >(context, &parameters, Proxy, 1);
//        cpp_objects[142] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::plus<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[143] = new FrameLib_Expand<FrameLib_BinaryOp<std::__1::multiplies<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[144] = new FrameLib_Expand<FrameLib_Constant>(context, &parameters, Proxy, 1);
//        cpp_objects[145] = new FrameLib_Expand<FrameLib_Untag>(context, &parameters, Proxy, 1);
//        cpp_objects[146] = new FrameLib_Expand<FrameLib_PerBlock>(context, &parameters, Proxy, 1);
//        cpp_objects[147] = new FrameLib_Expand<FrameLib_Gaussian>(context, &parameters, Proxy, 1);
//        cpp_objects[148] = new FrameLib_Expand<FrameLib_Pad>(context, &parameters, Proxy, 1);
//        cpp_objects[149] = new FrameLib_Expand<FrameLib_BinaryOp<FrameLib_Binary_Ops::absDiff> >(context, &parameters, Proxy, 1);
//        cpp_objects[150] = new FrameLib_Expand<FrameLib_Register>(context, &parameters, Proxy, 1);
//        cpp_objects[151] = new FrameLib_Expand<FrameLib_UnaryOp<std::__1::logical_not<double> > >(context, &parameters, Proxy, 1);
//        cpp_objects[152] = new FrameLib_Expand<FrameLib_Spatial>(context, &parameters, Proxy, 1);
//        cpp_objects[153] = new FrameLib_Expand<FrameLib_CombineTags>(context, &parameters, Proxy, 1);
//        cpp_objects[154] = new FrameLib_Pack(context, &parameters, Proxy, 1);
//        cpp_objects[155] = new FrameLib_Expand<FrameLib_AccumPoint>(context, &parameters, Proxy, 1);
//        cpp_objects[156] = new FrameLib_Expand<FrameLib_Vector<&statProduct<double const*>, false> >(context, &parameters, Proxy, 1);
//        cpp_objects[157] = new FrameLib_Expand<FrameLib_iFFT>(context, &parameters, Proxy, 1);
//        cpp_objects[158] = new FrameLib_Expand<FrameLib_Shift>(context, &parameters, Proxy, 1);
//        cpp_objects[159] = new FrameLib_Expand<FrameLib_Convolve>(context, &parameters, Proxy, 1);


        
        for (int i = 0; i < cpp_objects.size(); i++) {
            enum InfoFlags { kInfoDesciption = 0x01, kInfoInputs = 0x02, kInfoOutputs = 0x04, kInfoParameters = 0x08 };
            bool verbose = true;
            std::ofstream myfile;
            std::string sp = " "; // code is more readable with sp rather than " "
            std::string object = objectNames[i];
            myfile.open ("/Users/jamesbradbury/" + object + ".maxref.xml");
            std::string object_category = "!@#@#$";
            std::string object_keywords = "boilerplate keywords";
            std::string object_info;
            std::string object_description;
            std::string object_digest;
            // store some indentations - groups of 4 spaces
            std::string tab_1 = "    ";
            std::string tab_2 = "        ";
            std::string tab_3 = "            ";
            std::string tab_4 = "                ";
            
            // Write some stuff at the top of every xml file.
            myfile << "<?xml version='1.0' encoding='utf-8' standalone='yes'?> \n" ;
            myfile << "<?xml-stylesheet href='./_c74_ref.xsl' type='text/xsl'?> \n \n" ;
            myfile << "<c74object name='" << object << "' " << "module='FrameLib' " << "category=" << "'" << object_category << std::string("'> ") << "\n \n";
            
            // Description
            
            // split the object info into a description and a digest
            object_info = cpp_objects[i]->objectInfo(verbose);
            std::size_t pos = object_info.find(":");
            object_digest = object_info.substr(0, pos);
            object_description = object_info.substr(pos + 1);
            
            // now write that info into sections
            myfile << tab_1 + "<digest> \n";
            myfile << tab_2 + object_digest + "." + "\n";
            myfile << tab_1 + "</digest> \n \n";
            
            myfile << tab_1 + "<description> \n";
            myfile << tab_2 + object_description + "\n";
            myfile << tab_1 + "</description> \n \n";
            
            // Parameters
            
            // If object has no parameters create the 'no parameters template'
            const FrameLib_Parameters *params = cpp_objects[i]->getParameters();
            
            if (!params || !params->size()) {
                myfile << tab_1 + "<misc name = 'Parameters'> \n";
                myfile << tab_2 + "<entry> \n";
                myfile << tab_3 + "<description> \n";
                myfile << tab_4 + "This object has no parameters. \n";
                myfile << tab_3 + "</description> \n";
                myfile << tab_2 + "</entry> \n";
                myfile << tab_1 + "</misc> \n \n";
            }
            
            // Loop over parameters
            
            if (params->size() != 0) {
                myfile << tab_1 + "<misc name = 'Parameters'> \n \n"; // Write parameters tag to start misc section named Parameters
                for (long i = 0; params && i < params->size(); i++)
                {
                    std::string param_num = std::to_string(i);
                    FrameLib_Parameters::Type type = params->getType(i);
                    FrameLib_Parameters::NumericType numericType = params->getNumericType(i); // remove possibly. its not being used
                    std::string defaultStr = params->getDefaultString(i);
                    
                    // Name, type and default value
                    
                    if (defaultStr.size()) {
                        
                        myfile << tab_2 + "<" + "entry name = " + "'" + param_num + ". " + "/" + params->getName(i) + sp + "[" + params->getTypeString(i) + "]' " + ">" + "\n";
                    }
                    else {
                        
                        myfile << tab_2 + "<" + "entry name = " + "'" + param_num + ". " + "/" + params->getName(i) + sp + "[" + params->getTypeString(i) + "]' " + ">" + "\n";
                    }
                    // Construct the description
                    myfile << tab_3 + "<description> \n";
                    myfile << tab_4 + params->getInfo(i);
                    
                    // Verbose - arguments, range (for numeric types), enum items (for enums), array sizes (for arrays), description
                    if (verbose)
                    {
                        if (type == FrameLib_Parameters::kEnum){
                            
                            myfile << "<br></br> \n" ; // if enum put a break big break between description and the enum options
                            
                            for (long j = 0; j <= params->getMax(i); j++) {
                                std::string enum_param_num = std::to_string(j);
                                if (j == params->getMax(i))
                                    
                                    myfile << tab_4 + "<bullet>[" + enum_param_num + "]" + " - " + params->getItemString(i, j) + "</bullet>";
                                
                                else if (j != params->getMax(i))
                                    
                                    myfile << tab_4 + "<bullet>[" + enum_param_num + "]" + " - " + params->getItemString(i, j) + "</bullet> \n";
                            }
                        }
                        
                        myfile << "\n" + tab_3 + "</description> \n";
                        myfile << tab_2 + "</entry> \n \n";
                        
                    }
                }
                myfile << tab_1 + "</misc> \n \n";
            }
            // All the tail end stuff is going to here like see also and key words. //
            
            // Metadata //
            myfile << tab_1 + "<metadatalist> \n";
            myfile << tab_2 + "<metadata name='author'>Alex Harker</metadata> \n";
            myfile << tab_2 + "<metadata name='tag'>FrameLib</metadata> \n";
            myfile << tab_2 + "<metadata name='tag'>" + object_category + "</metadata> \n";
            myfile << tab_1 + "</metadatalist> \n \n";
            
            // Seealso //
            myfile << tab_1 + "<seealsolist> \n";
            myfile << tab_1 + "</seealsolist> \n";
            
            // Keywords //
            myfile << tab_1 + "<misc name = 'Discussion'> \n";
            myfile << tab_2 + "<entry name = 'Keywords'> \n";
            myfile << tab_3 + "<description> \n";
            myfile << tab_4 + object_keywords + "\n";
            myfile << tab_3 + "</description> \n";
            myfile << tab_2 + "</entry> \n";
            myfile << tab_1 + "</misc> \n \n";
            myfile << "</c74object>";

            myfile.close();
            delete cpp_objects[i];
        }
        cpp_objects.clear();
        delete Proxy;
        FrameLib_Global::release(&Global);
    }
        
int main() 
{
    write_info();
    return 0;
}








