
#ifndef FRAMELIB_OBJECTS_H
#define FRAMELIB_OBJECTS_H

#include "FrameLib_Templates.h"
#include "FrameLib_Vector_Functions.h"

// Unary

// Info specialisations

template<> const char *FrameLib_Unary <acos>::getOpString() { return "arccosine (in radians)"; }
template<> const char *FrameLib_Unary <asin>::getOpString() { return "arcsine (in radians)"; }
template<> const char *FrameLib_Unary <atan>::getOpString() { return "arctangent (in radians)"; }
template<> const char *FrameLib_Unary <cos>::getOpString() { return "cosine (for arguments in radians)"; }
template<> const char *FrameLib_Unary <sin>::getOpString() { return "sine (for arguments in radians)"; }
template<> const char *FrameLib_Unary <tan>::getOpString() { return "tangent (for arguments in radians)"; }
template<> const char *FrameLib_Unary <acosh>::getOpString() { return "hyperbolic arccosine"; }
template<> const char *FrameLib_Unary <asinh>::getOpString() { return "hyperbolic arcsine"; }
template<> const char *FrameLib_Unary <atanh>::getOpString() { return "hyperbolic arctangent"; }
template<> const char *FrameLib_Unary <cosh>::getOpString() { return "hyperbolic cosine"; }
template<> const char *FrameLib_Unary <sinh>::getOpString() { return "hyperbolic sine"; }
template<> const char *FrameLib_Unary <tanh>::getOpString() { return "hyperbolic tangent"; }

template<> const char *FrameLib_Unary <log>::getOpString() { return "logarithm base e"; }
template<> const char *FrameLib_Unary <log2>::getOpString() { return "logarithm base 2"; }
template<> const char *FrameLib_Unary <log10>::getOpString() { return "logarithm base 10"; }
template<> const char *FrameLib_Unary <exp>::getOpString() { return "exponent base e (e to the power of)"; }
template<> const char *FrameLib_Unary <exp2>::getOpString() { return "exponent base 2 (2 to the power of)"; }

template<> const char *FrameLib_Unary <fabs>::getOpString() { return "absolute value"; }
template<> const char *FrameLib_Unary <ceil>::getOpString() { return "ceiling value"; }
template<> const char *FrameLib_Unary <floor>::getOpString() { return "floor value"; }
template<> const char *FrameLib_Unary <round>::getOpString() { return "rounded value"; }
template<> const char *FrameLib_Unary <trunc>::getOpString() { return "truncated value"; }

template<> const char *FrameLib_Unary <sqrt>::getOpString() { return "square root"; }
template<> const char *FrameLib_Unary <cbrt>::getOpString() { return "cube root"; }
template<> const char *FrameLib_Unary <erf>::getOpString() { return "error function"; }
template<> const char *FrameLib_Unary <erfc>::getOpString() { return "complimentary error function"; }

// Type definitions

typedef FrameLib_Unary <acos>   FrameLib_Acos;
typedef FrameLib_Unary <asin>   FrameLib_Asin;
typedef FrameLib_Unary <atan>   FrameLib_Atan;
typedef FrameLib_Unary <cos>    FrameLib_Cos;
typedef FrameLib_Unary <sin>    FrameLib_Sin;
typedef FrameLib_Unary <tan>    FrameLib_Tan;
typedef FrameLib_Unary <acosh>  FrameLib_Acosh;
typedef FrameLib_Unary <asinh>  FrameLib_Asinh;
typedef FrameLib_Unary <atanh>  FrameLib_Atanh;
typedef FrameLib_Unary <cosh>   FrameLib_Cosh;
typedef FrameLib_Unary <sinh>   FrameLib_Sinh;
typedef FrameLib_Unary <tanh>   FrameLib_Tanh;

typedef FrameLib_Unary <log>    FrameLib_Log;
typedef FrameLib_Unary <log2>   FrameLib_Log2;
typedef FrameLib_Unary <log10>  FrameLib_Log10;
typedef FrameLib_Unary <exp>    FrameLib_Exp;
typedef FrameLib_Unary <exp2>   FrameLib_Exp2;

typedef FrameLib_Unary <fabs>   FrameLib_Abs;
typedef FrameLib_Unary <ceil>   FrameLib_Ceil;
typedef FrameLib_Unary <floor>  FrameLib_Floor;
typedef FrameLib_Unary <round>  FrameLib_Round;
typedef FrameLib_Unary <trunc>  FrameLib_Trunc;

typedef FrameLib_Unary <sqrt>   FrameLib_Sqrt;
typedef FrameLib_Unary <cbrt>   FrameLib_Cbrt;
typedef FrameLib_Unary <erf>    FrameLib_Erf;
typedef FrameLib_Unary <erfc>   FrameLib_Erfc;

// Binary (operators)

// Info specialisations

template<> const char *FrameLib_BinaryOp <std::plus<double> >::getDescriptionString() { return "Adds frames"; }
template<> const char *FrameLib_BinaryOp <std::minus<double> >::getDescriptionString() { return "Subtracts the right input frame from the left input frame"; }
template<> const char *FrameLib_BinaryOp <std::multiplies<double> >::getDescriptionString() { return "Multiplies frames"; }
template<> const char *FrameLib_BinaryOp <std::divides<double> >::getDescriptionString() { return "Divides the left input frame by the right input frame"; }

template<> const char *FrameLib_BinaryOp <std::equal_to<double> >::getDescriptionString() { return "Compares frame values for equality (output is 1 for true and 0 for false)"; }
template<> const char *FrameLib_BinaryOp <std::not_equal_to<double> >::getDescriptionString() { return "Compares frame values for inequality (output is 1 for true and 0 for false)"; }
template<> const char *FrameLib_BinaryOp <std::greater<double> >::getDescriptionString() 
{ return "Calculates if left input frame values are greater than right input frame values (output is 1 for true and 0 for false)"; }
template<> const char *FrameLib_BinaryOp <std::less<double> >::getDescriptionString()
{ return "Calculates if left input frame values are less than right input frame values (output is 1 for true and 0 for false)"; }
template<> const char *FrameLib_BinaryOp <std::greater_equal<double> >::getDescriptionString()
{ return "Calculates if left input frame values are greater than or equal to right input frame values (output is 1 for true and 0 for false)"; }
template<> const char *FrameLib_BinaryOp <std::less_equal<double> >::getDescriptionString()
{ return "Calculates if left input frame values are greater than or equal to right input frame values (output is 1 for true and 0 for false)"; }

// Type definitions

typedef FrameLib_BinaryOp <std::plus<double> >          FrameLib_Plus;
typedef FrameLib_BinaryOp <std::minus<double> >         FrameLib_Minus;
typedef FrameLib_BinaryOp <std::multiplies<double> >    FrameLib_Multiply;
typedef FrameLib_BinaryOp <std::divides<double> >       FrameLib_Divide;

typedef FrameLib_BinaryOp <std::equal_to<double> >      FrameLib_Equal;
typedef FrameLib_BinaryOp <std::not_equal_to<double> >  FrameLib_NotEqual;
typedef FrameLib_BinaryOp <std::greater<double> >       FrameLib_GreaterThan;
typedef FrameLib_BinaryOp <std::less<double> >          FrameLib_LessThan;
typedef FrameLib_BinaryOp <std::greater_equal<double> > FrameLib_GreaterThanEqual;
typedef FrameLib_BinaryOp <std::less_equal<double> >    FrameLib_LessThanEqual;

// Binary (functions)

// Info specialisations

template<> const char *FrameLib_Binary <pow>::getDescriptionString() { return "Calculates left input frame values to the power of right input frame values"; }
template<> const char *FrameLib_Binary <atan2>::getDescriptionString() { return "Computes the arctangent using two arguments (pairs of values from the two input frames)"; }
template<> const char *FrameLib_Binary <hypot>::getDescriptionString() { return "For each pair of input values calculates the hypotenuse of a right-angled triangle with sides of the given lengths"; }
template<> const char *FrameLib_Binary <copysign>::getDescriptionString() { return "Outputs a frame with the magnitude values of the left input frame and the sign values of right input frame"; }
template<> const char *FrameLib_Binary <fmin>::getDescriptionString() { return "Calculates the minimums of pairs of values in the two input frames"; }
template<> const char *FrameLib_Binary <fmax>::getDescriptionString() { return "Calculates the maximums of pairs of values in the two input frames"; }
template<> const char *FrameLib_Binary <fdim>::getDescriptionString() { return "Calculates the absolute difference between values in two input frames"; }

typedef FrameLib_Binary <pow>       FrameLib_Pow;
typedef FrameLib_Binary <atan2>     FrameLib_Atan2;
typedef FrameLib_Binary <hypot>     FrameLib_Hypot;
typedef FrameLib_Binary <copysign>  FrameLib_CopySign;
typedef FrameLib_Binary <fmin>      FrameLib_Min;
typedef FrameLib_Binary <fmax>      FrameLib_Max;
typedef FrameLib_Binary <fdim>      FrameLib_Diff;

// Vector

// Info specialisations

template<> const char *FrameLib_Vector <vectorMin>::getOpString() { return "minimum"; }
template<> const char *FrameLib_Vector <vectorMax>::getOpString() { return "maximum"; }
template<> const char *FrameLib_Vector <vectorSum>::getOpString() { return "sum"; }
template<> const char *FrameLib_Vector <vectorProduct>::getOpString() { return "product"; }
template<> const char *FrameLib_Vector <vectorMean>::getOpString() { return "mean"; }
template<> const char *FrameLib_Vector <vectorGeometricMean>::getOpString() { return "geometric mean"; }
template<> const char *FrameLib_Vector <vectorLength>::getOpString() { return "length"; }

// Type definitions

typedef FrameLib_Vector <vectorMin>             FrameLib_VectorMin;
typedef FrameLib_Vector <vectorMax>             FrameLib_VectorMax;
typedef FrameLib_Vector <vectorSum>             FrameLib_Sum;
typedef FrameLib_Vector <vectorProduct>         FrameLib_Product;
typedef FrameLib_Vector <vectorMean>            FrameLib_Mean;
typedef FrameLib_Vector <vectorGeometricMean>   FrameLib_GeometricMean;
typedef FrameLib_Vector <vectorLength>          FrameLib_Length;

#endif
