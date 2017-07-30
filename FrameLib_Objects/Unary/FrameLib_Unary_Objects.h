
#ifndef FRAMELIB_UNARY_OBJECTS_H
#define FRAMELIB_UNARY_OBJECTS_H

#include "FrameLib_Unary_Template.h"

// Info specialisations

template<> const char *FrameLib_Unary<acos>::getOpString()  { return "arccosine (in radians)"; }
template<> const char *FrameLib_Unary<asin>::getOpString()  { return "arcsine (in radians)"; }
template<> const char *FrameLib_Unary<atan>::getOpString()  { return "arctangent (in radians)"; }
template<> const char *FrameLib_Unary<cos>::getOpString()   { return "cosine (for arguments in radians)"; }
template<> const char *FrameLib_Unary<sin>::getOpString()   { return "sine (for arguments in radians)"; }
template<> const char *FrameLib_Unary<tan>::getOpString()   { return "tangent (for arguments in radians)"; }
template<> const char *FrameLib_Unary<acosh>::getOpString() { return "hyperbolic arccosine"; }
template<> const char *FrameLib_Unary<asinh>::getOpString() { return "hyperbolic arcsine"; }
template<> const char *FrameLib_Unary<atanh>::getOpString() { return "hyperbolic arctangent"; }
template<> const char *FrameLib_Unary<cosh>::getOpString()  { return "hyperbolic cosine"; }
template<> const char *FrameLib_Unary<sinh>::getOpString()  { return "hyperbolic sine"; }
template<> const char *FrameLib_Unary<tanh>::getOpString()  { return "hyperbolic tangent"; }

template<> const char *FrameLib_Unary<log>::getOpString()   { return "logarithm base e"; }
template<> const char *FrameLib_Unary<log2>::getOpString()  { return "logarithm base 2"; }
template<> const char *FrameLib_Unary<log10>::getOpString() { return "logarithm base 10"; }
template<> const char *FrameLib_Unary<exp>::getOpString()   { return "exponent base e (e to the power of)"; }
template<> const char *FrameLib_Unary<exp2>::getOpString()  { return "exponent base 2 (2 to the power of)"; }

template<> const char *FrameLib_Unary<fabs>::getOpString()  { return "absolute value"; }
template<> const char *FrameLib_Unary<ceil>::getOpString()  { return "ceiling value"; }
template<> const char *FrameLib_Unary<floor>::getOpString() { return "floor value"; }
template<> const char *FrameLib_Unary<round>::getOpString() { return "rounded value"; }
template<> const char *FrameLib_Unary<trunc>::getOpString() { return "truncated value"; }

template<> const char *FrameLib_Unary<sqrt>::getOpString()  { return "square root"; }
template<> const char *FrameLib_Unary<cbrt>::getOpString()  { return "cube root"; }
template<> const char *FrameLib_Unary<erf>::getOpString()   { return "error function"; }
template<> const char *FrameLib_Unary<erfc>::getOpString()  { return "complimentary error function"; }

// Type definitions

typedef FrameLib_Unary<acos>    FrameLib_Acos;
typedef FrameLib_Unary<asin>    FrameLib_Asin;
typedef FrameLib_Unary<atan>    FrameLib_Atan;
typedef FrameLib_Unary<cos>     FrameLib_Cos;
typedef FrameLib_Unary<sin>     FrameLib_Sin;
typedef FrameLib_Unary<tan>     FrameLib_Tan;
typedef FrameLib_Unary<acosh>   FrameLib_Acosh;
typedef FrameLib_Unary<asinh>   FrameLib_Asinh;
typedef FrameLib_Unary<atanh>   FrameLib_Atanh;
typedef FrameLib_Unary<cosh>    FrameLib_Cosh;
typedef FrameLib_Unary<sinh>    FrameLib_Sinh;
typedef FrameLib_Unary<tanh>    FrameLib_Tanh;

typedef FrameLib_Unary<log>     FrameLib_Log;
typedef FrameLib_Unary<log2>    FrameLib_Log2;
typedef FrameLib_Unary<log10>   FrameLib_Log10;
typedef FrameLib_Unary<exp>     FrameLib_Exp;
typedef FrameLib_Unary<exp2>    FrameLib_Exp2;

typedef FrameLib_Unary<fabs>    FrameLib_Abs;
typedef FrameLib_Unary<ceil>    FrameLib_Ceil;
typedef FrameLib_Unary<floor>   FrameLib_Floor;
typedef FrameLib_Unary<round>   FrameLib_Round;
typedef FrameLib_Unary<trunc>   FrameLib_Trunc;

typedef FrameLib_Unary<sqrt>    FrameLib_Sqrt;
typedef FrameLib_Unary<cbrt>    FrameLib_Cbrt;
typedef FrameLib_Unary<erf>     FrameLib_Erf;
typedef FrameLib_Unary<erfc>    FrameLib_Erfc;

#endif
