
#ifndef FRAMELIB_BINARY_OBJECTS_H
#define FRAMELIB_BINARY_OBJECTS_H

#include "FrameLib_Binary_Template.h"

#include <functional>

// Absolute diff functor

namespace FrameLib_Binary_Ops
{
    struct absDiff { double operator()(double x, double y) { return std::abs(x-y); } };
}

// Binary (operators)

// Info specialisations

template<> inline const char *FrameLib_BinaryOp<std::plus<double>>::getDescriptionString()
{ return "Adds values in the two input frames"; }

template<> inline const char *FrameLib_BinaryOp<std::minus<double>>::getDescriptionString()
{ return "Subtracts the right input frame values from the left input frame values"; }

template<> inline const char *FrameLib_BinaryOp<std::multiplies<double>>::getDescriptionString()
{ return "Multiplies values in the two input frames"; }

template<> inline const char *FrameLib_BinaryOp<std::divides<double>>::getDescriptionString()
{ return "Divides the left input frame values by the right input frame values"; }

template<> inline const char *FrameLib_BinaryOp<std::equal_to<double>>::getDescriptionString()
{ return "Compares frame values for equality (output is 1 for true and 0 for false)"; }

template<> inline const char *FrameLib_BinaryOp<std::not_equal_to<double>>::getDescriptionString()
{ return "Compares frame values for inequality (output is 1 for true and 0 for false)"; }

template<> inline const char *FrameLib_BinaryOp<std::greater<double>>::getDescriptionString()
{ return "Calculates if left input frame values are greater than right input frame values (output is 1 for true and 0 for false)"; }

template<> inline const char *FrameLib_BinaryOp<std::less<double>>::getDescriptionString()
{ return "Calculates if left input frame values are less than right input frame values (output is 1 for true and 0 for false)"; }

template<> inline const char *FrameLib_BinaryOp<std::greater_equal<double>>::getDescriptionString()
{ return "Calculates if left input frame values are greater than or equal to right input frame values (output is 1 for true and 0 for false)"; }

template<> inline const char *FrameLib_BinaryOp<std::less_equal<double>>::getDescriptionString()
{ return "Calculates if left input frame values are greater than or equal to right input frame values (output is 1 for true and 0 for false)"; }

template<> inline const char *FrameLib_BinaryOp<std::logical_and<double>>::getDescriptionString()
{ return "Calculates the logical and of the left and right input frame values"; }

template<> inline const char *FrameLib_BinaryOp<std::logical_or<double>>::getDescriptionString()
{ return "Calculates the logical or of the left and right input frame values"; }

template<> inline const char *FrameLib_BinaryOp<FrameLib_Binary_Ops::absDiff>::getDescriptionString()
{ return "Calculates the absolute differences between values in the two input frames"; }


// Type definitions

typedef FrameLib_BinaryOp<std::plus<double>>            FrameLib_Plus;
typedef FrameLib_BinaryOp<std::minus<double>>           FrameLib_Minus;
typedef FrameLib_BinaryOp<std::multiplies<double>>      FrameLib_Multiply;
typedef FrameLib_BinaryOp<std::divides<double>>         FrameLib_Divide;

typedef FrameLib_BinaryOp<std::equal_to<double>>        FrameLib_Equal;
typedef FrameLib_BinaryOp<std::not_equal_to<double>>    FrameLib_NotEqual;
typedef FrameLib_BinaryOp<std::greater<double>>         FrameLib_GreaterThan;
typedef FrameLib_BinaryOp<std::less<double>>            FrameLib_LessThan;
typedef FrameLib_BinaryOp<std::greater_equal<double>>   FrameLib_GreaterThanEqual;
typedef FrameLib_BinaryOp<std::less_equal<double>>      FrameLib_LessThanEqual;

typedef FrameLib_BinaryOp<std::logical_and<double>>     FrameLib_LogicalAnd;
typedef FrameLib_BinaryOp<std::logical_or<double>>      FrameLib_LogicalOr;

typedef FrameLib_BinaryOp<FrameLib_Binary_Ops::absDiff> FrameLib_Diff;

// Binary (functions)

// Info specialisations

template<> inline const char *FrameLib_Binary<pow>::getDescriptionString()
{ return "Calculates left input frame values to the power of right input frame values"; }

template<> inline const char *FrameLib_Binary<atan2>::getDescriptionString()
{ return "For each pair of input values calculates the arctangent using two arguments (left operand is y, right operand is x)"; }

template<> inline const char *FrameLib_Binary<hypot>::getDescriptionString()
{ return "For each pair of input values calculates the hypotenuse of a right-angled triangle with sides of the given lengths"; }

template<> inline const char *FrameLib_Binary<copysign>::getDescriptionString()
{ return "Outputs a frame with the magnitude values of the left input frame and the sign values of right input frame"; }

template<> inline const char *FrameLib_Binary<fmin>::getDescriptionString()
{ return "Calculates the minimums of pairs of values in the two input frames"; }

template<> inline const char *FrameLib_Binary<fmax>::getDescriptionString()
{ return "Calculates the maximums of pairs of values in the two input frames"; }

template<> inline const char *FrameLib_Binary<fmod>::getDescriptionString()
{ return "Calculates the left input frame values modulo the right input frame values"; }

typedef FrameLib_Binary<pow>            FrameLib_Pow;
typedef FrameLib_Binary<atan2>          FrameLib_Atan2;
typedef FrameLib_Binary<hypot>          FrameLib_Hypot;
typedef FrameLib_Binary<copysign>       FrameLib_CopySign;
typedef FrameLib_Binary<fmin>           FrameLib_Min;
typedef FrameLib_Binary<fmax>           FrameLib_Max;
typedef FrameLib_Binary<fmod>           FrameLib_Modulo;

#endif
