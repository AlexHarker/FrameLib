
#ifndef FRAMELIB_TYPEALIASES_H
#define FRAMELIB_TYPEALIASES_H

#include "FrameLib_SerialiseGraph.h"

struct FrameLib_TypeAliases
{
    static std::vector<FrameLib_StringReplace> makeReplaceStrings()
    {
        return
        {
            { ">", " >" },
            { "FrameLib_Biquad",            "FrameLib_Filter<FrameLib_Filters::Biquad>" },
            { "FrameLib_BiquadCoeff",       "FrameLib_Filter<FrameLib_Filters::BiquadCoeff>" },
            { "FrameLib_OnePole",           "FrameLib_Filter<FrameLib_Filters::OnePole>" },
            { "FrameLib_OnePoleZero",       "FrameLib_Filter<FrameLib_Filters::OnePoleZero>" },
            { "FrameLib_SVF",               "FrameLib_Filter<FrameLib_Filters::SVF>" },
            { "FrameLib_Acos",              "FrameLib_UnaryOp<Unary_Functor<&acos>>" },
            { "FrameLib_Asin",              "FrameLib_UnaryOp<Unary_Functor<&asin>>" },
            { "FrameLib_Atan",              "FrameLib_UnaryOp<Unary_Functor<&atan>>" },
            { "FrameLib_Cos",               "FrameLib_UnaryOp<Unary_Functor<&cos>>" },
            { "FrameLib_Sin",               "FrameLib_UnaryOp<Unary_Functor<&sin>>" },
            { "FrameLib_Tan",               "FrameLib_UnaryOp<Unary_Functor<&tan>>" },
            { "FrameLib_Acosh",             "FrameLib_UnaryOp<Unary_Functor<&acosh>>" },
            { "FrameLib_Asinh",             "FrameLib_UnaryOp<Unary_Functor<&asinh>>" },
            { "FrameLib_Atanh",             "FrameLib_UnaryOp<Unary_Functor<&atanh>>" },
            { "FrameLib_Cosh",              "FrameLib_UnaryOp<Unary_Functor<&cosh>>" },
            { "FrameLib_Sinh",              "FrameLib_UnaryOp<Unary_Functor<&sinh>>" },
            { "FrameLib_Tanh",              "FrameLib_UnaryOp<Unary_Functor<&tanh>>" },
            { "FrameLib_Log",               "FrameLib_UnaryOp<Unary_Functor<&log>>" },
            { "FrameLib_Log2",              "FrameLib_UnaryOp<Unary_Functor<&log2>>" },
            { "FrameLib_Log10",             "FrameLib_UnaryOp<Unary_Functor<&log10>>" },
            { "FrameLib_Exp",               "FrameLib_UnaryOp<Unary_Functor<&exp>>" },
            { "FrameLib_Exp2",              "FrameLib_UnaryOp<Unary_Functor<&exp2>>" },
            { "FrameLib_Abs",               "FrameLib_UnaryOp<Unary_Functor<&fabs>>" },
            { "FrameLib_Ceil",              "FrameLib_UnaryOp<Unary_Functor<&ceil>>" },
            { "FrameLib_Floor",             "FrameLib_UnaryOp<Unary_Functor<&floor>>" },
            { "FrameLib_Round",             "FrameLib_UnaryOp<Unary_Functor<&round>>" },
            { "FrameLib_Trunc",             "FrameLib_UnaryOp<Unary_Functor<&trunc>>" },
            { "FrameLib_Sqrt",              "FrameLib_UnaryOp<Unary_Functor<&sqrt>>" },
            { "FrameLib_Cbrt",              "FrameLib_UnaryOp<Unary_Functor<&cbrt>>" },
            { "FrameLib_Erf",               "FrameLib_UnaryOp<Unary_Functor<&erf>>" },
            { "FrameLib_Erfc",              "FrameLib_UnaryOp<Unary_Functor<&erfc>>" },
            { "FrameLib_Not",               "FrameLib_UnaryOp<std::logical_not<double>>" },
            { "FrameLib_Add",               "FrameLib_BinaryOp<std::plus<double>>" },
            { "FrameLib_Subtract",          "FrameLib_BinaryOp<std::minus<double>>" },
            { "FrameLib_Multiply",          "FrameLib_BinaryOp<std::multiplies<double>>" },
            { "FrameLib_Divide",            "FrameLib_BinaryOp<std::divides<double>>" },
            { "FrameLib_Equal",             "FrameLib_BinaryOp<std::equal_to<double>>" },
            { "FrameLib_NotEqual",          "FrameLib_BinaryOp<std::not_equal_to<double>>" },
            { "FrameLib_GreaterThan",       "FrameLib_BinaryOp<std::greater<double>>" },
            { "FrameLib_LessThan",          "FrameLib_BinaryOp<std::less<double>>" },
            { "FrameLib_GreaterThanEqual",  "FrameLib_BinaryOp<std::greater_equal<double>>" },
            { "FrameLib_LessThanEqual",     "FrameLib_BinaryOp<std::less_equal<double>>" },
            { "FrameLib_LogicalAnd",        "FrameLib_BinaryOp<std::logical_and<double>>" },
            { "FrameLib_LogicalOr",         "FrameLib_BinaryOp<std::logical_or<double>>" },
            { "FrameLib_Diff",              "FrameLib_BinaryOp<FrameLib_Binary_Ops::absDiff>" },
            { "FrameLib_Pow",               "FrameLib_BinaryOp<Binary_Functor<&pow>>" },
            { "FrameLib_Atan2",             "FrameLib_BinaryOp<Binary_Functor<&atan2>>" },
            { "FrameLib_Hypot",             "FrameLib_BinaryOp<Binary_Functor<&hypot>>" },
            { "FrameLib_CopySign",          "FrameLib_BinaryOp<Binary_Functor<&copysign>>" },
            { "FrameLib_Min",               "FrameLib_BinaryOp<Binary_Functor<&fmin>>" },
            { "FrameLib_Max",               "FrameLib_BinaryOp<Binary_Functor<&fmax>>" },
            { "FrameLib_Modulo",            "FrameLib_BinaryOp<Binary_Functor<&fmod>>" },
            { "FrameLib_Clip",              "FrameLib_TernaryOp<Ternary_Functor<&FrameLib_Ternary_Ops::clip<double>>>" },
            { "FrameLib_Wrap",              "FrameLib_TernaryOp<Ternary_Functor<&FrameLib_Ternary_Ops::wrap<double>>>" },
            { "FrameLib_Fold",              "FrameLib_TernaryOp<Ternary_Functor<&FrameLib_Ternary_Ops::fold<double>>>" },
            { "FrameLib_Complex_Cos",       "FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::cos<double>>>" },
            { "FrameLib_Complex_Sin",       "FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::sin<double>>>" },
            { "FrameLib_Complex_Tan",       "FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::tan<double>>>" },
            { "FrameLib_Complex_Cosh",      "FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::cosh<double>>>" },
            { "FrameLib_Complex_Sinh",      "FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::sinh<double>>>" },
            { "FrameLib_Complex_Tanh",      "FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::tanh<double>>>" },
            { "FrameLib_Complex_Log",       "FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::log<double>>>" },
            { "FrameLib_Complex_Log10",     "FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::log10<double>>>" },
            { "FrameLib_Complex_Exp",       "FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::exp<double>>>" },
            { "FrameLib_Complex_Sqrt",      "FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::sqrt<double>>>" },
            { "FrameLib_Complex_Conj",      "FrameLib_Complex_UnaryOp<Complex_Unary_Functor<&std::conj<double>>>" },
            { "FrameLib_Complex_Add",       "FrameLib_Complex_BinaryOp<std::plus<std::complex<double>>>" },
            { "FrameLib_Complex_Subtract",  "FrameLib_Complex_BinaryOp<std::minus<std::complex<double>>>" },
            { "FrameLib_Complex_Multiply",  "FrameLib_Complex_BinaryOp<std::multiplies<std::complex<double>>>" },
            { "FrameLib_Complex_Divide",    "FrameLib_Complex_BinaryOp<std::divides<std::complex<double>>>" },
            { "FrameLib_Complex_Pow",       "FrameLib_Complex_BinaryOp<Complex_Binary_Functor<&std::pow<double>>>" },
            { "FrameLib_Cartopol",          "FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::CarToPol>" },
            { "FrameLib_Poltocar",          "FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::PolToCar>" },
            { "FrameLib_Length",            "FrameLib_Vector<&stat_length<double const*>,4>" },
            { "FrameLib_FrameMin",          "FrameLib_Vector<&stat_min<double const*>,2>" },
            { "FrameLib_FrameMax",          "FrameLib_Vector<&stat_max<double const*>,3>" },
            { "FrameLib_Sum",               "FrameLib_Vector<&stat_sum<double const*>,0>" },
            { "FrameLib_Product",           "FrameLib_Vector<&stat_product<double const*>,1>" },
            { "FrameLib_Mean",              "FrameLib_Vector<&stat_mean<double const*>,0>" },
            { "FrameLib_GeometricMean",     "FrameLib_Vector<&stat_geometric_mean<double const*>,0>" },
            { "FrameLib_StandardDeviation", "FrameLib_Vector<&stat_standard_deviation<double const*>,0>" },
            { "FrameLib_Centroid",          "FrameLib_Vector<&stat_centroid<double const*>,0>" },
            { "FrameLib_Spread",            "FrameLib_Vector<&stat_spread<double const*>,0>" },
            { "FrameLib_Skewness",          "FrameLib_Vector<&stat_skewness<double const*>,0>" },
            { "FrameLib_Kurtosis",          "FrameLib_Vector<&stat_kurtosis<double const*>,1>" },
            { "FrameLib_Flatness",          "FrameLib_Vector<&stat_flatness<double const*>,1>" },
            { "FrameLib_RMS",               "FrameLib_Vector<&stat_rms<double const*>,0>" },
            { "FrameLib_Crest",             "FrameLib_Vector<&stat_crest<double const*>,1>" },
            { "FrameLib_FrameMinPosition",  "FrameLib_Vector<&stat_min_position<double const*>,0>" },
            { "FrameLib_FrameMaxPosition",  "FrameLib_Vector<&stat_max_position<double const*>,0>" }
        };
    }
};

#endif /* FRAMELIB_TYPEALIASES_H */
