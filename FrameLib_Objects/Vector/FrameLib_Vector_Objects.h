
#ifndef FRAMELIB_VECTOR_OBJECTS_H
#define FRAMELIB_VECTOR_OBJECTS_H

#include "FrameLib_Vector_Template.h"
#include "../../FrameLib_Dependencies/Statistics.hpp"

// Info specialisations

template<> const char *FrameLib_Vector<statMin>::getOpString()            { return "minimum"; }
template<> const char *FrameLib_Vector<statMax>::getOpString()            { return "maximum"; }
template<> const char *FrameLib_Vector<statSum>::getOpString()            { return "sum"; }
template<> const char *FrameLib_Vector<statProduct>::getOpString()        { return "product"; }
template<> const char *FrameLib_Vector<statMean>::getOpString()           { return "mean"; }
template<> const char *FrameLib_Vector<statGeometricMean>::getOpString()  { return "geometric mean"; }
template<> const char *FrameLib_Vector<statLength>::getOpString()         { return "length"; }

// Type definitions

typedef FrameLib_Vector<statMin>              FrameLib_VectorMin;
typedef FrameLib_Vector<statMax>              FrameLib_VectorMax;
typedef FrameLib_Vector<statSum>              FrameLib_Sum;
typedef FrameLib_Vector<statProduct>          FrameLib_Product;
typedef FrameLib_Vector<statMean>             FrameLib_Mean;
typedef FrameLib_Vector<statGeometricMean>    FrameLib_GeometricMean;
typedef FrameLib_Vector<statLength>           FrameLib_Length;

#endif
