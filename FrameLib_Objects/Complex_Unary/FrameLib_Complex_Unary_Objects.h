
#ifndef FRAMELIB_COMPLEX_UNARY_OBJECTS_H
#define FRAMELIB_COMPLEX_UNARY_OBJECTS_H

#include "FrameLib_Complex_Unary_Template.h"

// Complex Unary (functions)

// Info specialisations

template<> const char *FrameLib_Complex_Unary<std::cos<double> >::getOpString()   { return "cosine (for arguments in radians)"; }
template<> const char *FrameLib_Complex_Unary<std::sin<double> >::getOpString()   { return "sine (for arguments in radians)"; }
template<> const char *FrameLib_Complex_Unary<std::tan<double> >::getOpString()   { return "tangent (for arguments in radians)"; }
template<> const char *FrameLib_Complex_Unary<std::cosh<double> >::getOpString()  { return "hyperbolic cosine"; }
template<> const char *FrameLib_Complex_Unary<std::sinh<double> >::getOpString()  { return "hyperbolic sine"; }
template<> const char *FrameLib_Complex_Unary<std::tanh<double> >::getOpString()  { return "hyperbolic tangent"; }

template<> const char *FrameLib_Complex_Unary<std::log<double> >::getOpString()   { return "logarithm base e"; }
template<> const char *FrameLib_Complex_Unary<std::log10<double> >::getOpString() { return "logarithm base 10"; }
template<> const char *FrameLib_Complex_Unary<std::exp<double> >::getOpString()   { return "exponent base e (e to the power of)"; }

template<> const char *FrameLib_Complex_Unary<std::sqrt<double> >::getOpString()  { return "square root"; }

template<> const char *FrameLib_Complex_Unary<std::conj<double> >::getOpString()  { return "complex conjugate"; }

// Type definitions

typedef FrameLib_Complex_Unary<std::cos<double> >     FrameLib_Complex_Cos;
typedef FrameLib_Complex_Unary<std::sin<double> >     FrameLib_Complex_Sin;
typedef FrameLib_Complex_Unary<std::tan<double> >     FrameLib_Complex_Tan;
typedef FrameLib_Complex_Unary<std::cosh<double> >    FrameLib_Complex_Cosh;
typedef FrameLib_Complex_Unary<std::sinh<double> >    FrameLib_Complex_Sinh;
typedef FrameLib_Complex_Unary<std::tanh<double> >    FrameLib_Complex_Tanh;

typedef FrameLib_Complex_Unary<std::log<double> >     FrameLib_Complex_Log;
typedef FrameLib_Complex_Unary<std::log10<double> >   FrameLib_Complex_Log10;
typedef FrameLib_Complex_Unary<std::exp<double> >     FrameLib_Complex_Exp;

typedef FrameLib_Complex_Unary<std::sqrt<double> >    FrameLib_Complex_Sqrt;

typedef FrameLib_Complex_Unary<std::conj<double> >    FrameLib_Complex_Conj;

#endif
