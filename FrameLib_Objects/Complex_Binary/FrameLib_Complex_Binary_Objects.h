
#ifndef FRAMELIB_COMPLEX_BINARY_OBJECTS_H
#define FRAMELIB_COMPLEX_BINARY_OBJECTS_H

#include "FrameLib_Complex_Binary_Template.h"

#include <functional>

// Complex Binary (operators)

// Info specialisations

template<> inline const char *FrameLib_Complex_BinaryOp<std::plus<std::complex<double>>>::getDescriptionString()
{ return "Adds complex values in the two pairs of input frames"; }

template<> inline const char *FrameLib_Complex_BinaryOp<std::minus<std::complex<double>>>::getDescriptionString()
{ return "Subtracts complex values in the right input frame pair from those in the left pair"; }

template<> inline const char *FrameLib_Complex_BinaryOp<std::multiplies<std::complex<double>>>::getDescriptionString()
{ return "Multiplies complex values in the two pairs of input frames"; }

template<> inline const char *FrameLib_Complex_BinaryOp<std::divides<std::complex<double>>>::getDescriptionString()
{ return "Divides complex values in the left pair of input frames by those in the right pair"; }

// Type definitions

typedef FrameLib_Complex_BinaryOp<std::plus<std::complex<double>>>              FrameLib_Complex_Add;
typedef FrameLib_Complex_BinaryOp<std::minus<std::complex<double>>>             FrameLib_Complex_Subtract;
typedef FrameLib_Complex_BinaryOp<std::multiplies<std::complex<double>>>        FrameLib_Complex_Multiply;
typedef FrameLib_Complex_BinaryOp<std::divides<std::complex<double>>>           FrameLib_Complex_Divide;

// Complex Binary (functions)

// Info specialisations

template<> inline const char *FrameLib_Complex_Binary<std::pow<double>>::getDescriptionString()
{ return "Raises left operand values to the power of right operand values (both complex)"; }
                                                                           
typedef FrameLib_Complex_Binary<std::pow<double>>       FrameLib_Complex_Pow;

#endif
