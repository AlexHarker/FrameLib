
#ifndef FRAMELIB_VECTOR_OBJECTS_H
#define FRAMELIB_VECTOR_OBJECTS_H

#include "FrameLib_Vector_Template.h"
#include "FrameLib_Vector_Functions.h"

// Info specialisations

template<> const char *FrameLib_Vector<vectorMin>::getOpString()            { return "minimum"; }
template<> const char *FrameLib_Vector<vectorMax>::getOpString()            { return "maximum"; }
template<> const char *FrameLib_Vector<vectorSum>::getOpString()            { return "sum"; }
template<> const char *FrameLib_Vector<vectorProduct>::getOpString()        { return "product"; }
template<> const char *FrameLib_Vector<vectorMean>::getOpString()           { return "mean"; }
template<> const char *FrameLib_Vector<vectorGeometricMean>::getOpString()  { return "geometric mean"; }
template<> const char *FrameLib_Vector<vectorLength>::getOpString()         { return "length"; }

// Type definitions

typedef FrameLib_Vector<vectorMin>              FrameLib_VectorMin;
typedef FrameLib_Vector<vectorMax>              FrameLib_VectorMax;
typedef FrameLib_Vector<vectorSum>              FrameLib_Sum;
typedef FrameLib_Vector<vectorProduct>          FrameLib_Product;
typedef FrameLib_Vector<vectorMean>             FrameLib_Mean;
typedef FrameLib_Vector<vectorGeometricMean>    FrameLib_GeometricMean;
typedef FrameLib_Vector<vectorLength>           FrameLib_Length;

#endif
