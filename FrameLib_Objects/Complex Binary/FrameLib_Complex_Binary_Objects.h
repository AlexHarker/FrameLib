
#ifndef FRAMELIB_COMPLEX_BINARY_OBJECTS_H
#define FRAMELIB_COMPLEX_BINARY_OBJECTS_H

#include "FrameLib_Complex_Binary_Template.h"

// Complex Binary (operators)

// Info specialisations

template<> const char *FrameLib_Complex_BinaryOp<std::plus<std::complex<double> > >::getDescriptionString()
{ return "Adds complex values in the two sets of input frames"; }

template<> const char *FrameLib_Complex_BinaryOp<std::minus<std::complex<double> > >::getDescriptionString()
{ return "Subtracts the right complex pair of input frame values from the left complex pair of input frame values"; }

template<> const char *FrameLib_Complex_BinaryOp<std::multiplies<std::complex<double> > >::getDescriptionString()
{ return "Multiplies complexvalues in the two sets of input frames"; }

template<> const char *FrameLib_Complex_BinaryOp<std::divides<std::complex<double> > >::getDescriptionString()
{ return "Divides the left complex pair of input frame values by the right complex pair of input frame values"; }

// Type definitions

typedef FrameLib_Complex_BinaryOp<std::plus<std::complex<double> > >            FrameLib_Complex_Plus;
typedef FrameLib_Complex_BinaryOp<std::minus<std::complex<double> > >           FrameLib_Complex_Minus;
typedef FrameLib_Complex_BinaryOp<std::multiplies<std::complex<double> > >      FrameLib_Complex_Multiply;
typedef FrameLib_Complex_BinaryOp<std::divides<std::complex<double> > >         FrameLib_Complex_Divide;

// Complex Binary (functions)

// Info specialisations

template<> const char *FrameLib_Complex_Binary<std::pow<double> >::getDescriptionString()
{ return "Calculates left complex pair of input frame values to the power of right complex pair of input frame values"; }

typedef FrameLib_Complex_Binary<std::pow<double> >          FrameLib_Complex_Pow;

#endif
