
#ifndef FRAMELIB_OBJECTS_H
#define FRAMELIB_OBJECTS_H

#include "FrameLib_Templates.h"
#include "FrameLib_Vector_Functions.h"

// Unary

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

typedef FrameLib_Binary <pow>       FrameLib_Pow;
typedef FrameLib_Binary <atan2>     FrameLib_Atan2;
typedef FrameLib_Binary <hypot>     FrameLib_Hypot;
typedef FrameLib_Binary <copysign>  FrameLib_CopySign;
typedef FrameLib_Binary <fmin>      FrameLib_Min;
typedef FrameLib_Binary <fmax>      FrameLib_Max;
typedef FrameLib_Binary <fdim>      FrameLib_Diff;

// Vector

typedef FrameLib_Vector <vectorMin>             FrameLib_VectorMin;
typedef FrameLib_Vector <vectorMax>             FrameLib_VectorMax;
typedef FrameLib_Vector <vectorSum>             FrameLib_Sum;
typedef FrameLib_Vector <vectorProduct>         FrameLib_Product;
typedef FrameLib_Vector <vectorMean>            FrameLib_Mean;
typedef FrameLib_Vector <vectorGeometricMean>   FrameLib_GeometricMean;
typedef FrameLib_Vector <vectorLength>          FrameLib_Length;

#endif
