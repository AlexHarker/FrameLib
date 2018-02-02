
#ifndef FRAMELIB_VECTOR_OBJECTS_H
#define FRAMELIB_VECTOR_OBJECTS_H

#include "FrameLib_Vector_Template.h"
#include "../../FrameLib_Dependencies/Statistics.hpp"

// Info specialisations

template<> const char *FrameLib_Vector<statLength, true>::getOpString()         { return "length"; }
template<> const char *FrameLib_Vector<statMin>::getOpString()                  { return "minimum"; }
template<> const char *FrameLib_Vector<statMax>::getOpString()                  { return "maximum"; }
template<> const char *FrameLib_Vector<statSum>::getOpString()                  { return "sum"; }
template<> const char *FrameLib_Vector<statProduct>::getOpString()              { return "product"; }
template<> const char *FrameLib_Vector<statMean>::getOpString()                 { return "mean"; }
template<> const char *FrameLib_Vector<statGeometricMean>::getOpString()        { return "geometric mean"; }
template<> const char *FrameLib_Vector<statStandardDeviation>::getOpString()    { return "standard deviation"; }
template<> const char *FrameLib_Vector<statSpread>::getOpString()               { return "spread"; }
template<> const char *FrameLib_Vector<statSkewness>::getOpString()             { return "skewness"; }
template<> const char *FrameLib_Vector<statKurtosis>::getOpString()             { return "kurtosis"; }
template<> const char *FrameLib_Vector<statFlatness>::getOpString()             { return "flatness"; }
template<> const char *FrameLib_Vector<statRMS>::getOpString()                  { return "rms"; }
template<> const char *FrameLib_Vector<statCrest>::getOpString()                { return "crest factor"; }
template<> const char *FrameLib_Vector<statArgMin>::getOpString()               { return "index of minimum"; }
template<> const char *FrameLib_Vector<statArgMax>::getOpString()               { return "index of maximum"; }

// Type definitions

typedef FrameLib_Vector<statLength, true>       FrameLib_Length;
typedef FrameLib_Vector<statMin>                FrameLib_VectorMin;
typedef FrameLib_Vector<statMax>                FrameLib_VectorMax;
typedef FrameLib_Vector<statSum>                FrameLib_Sum;
typedef FrameLib_Vector<statProduct>            FrameLib_Product;
typedef FrameLib_Vector<statMean>               FrameLib_Mean;
typedef FrameLib_Vector<statGeometricMean>      FrameLib_GeometricMean;
typedef FrameLib_Vector<statStandardDeviation>  FrameLib_StandardDeviation;
typedef FrameLib_Vector<statCentroid>           FrameLib_Centroid;
typedef FrameLib_Vector<statSpread>             FrameLib_Spread;
typedef FrameLib_Vector<statSkewness>           FrameLib_Skewness;
typedef FrameLib_Vector<statKurtosis>           FrameLib_Kurtosis;
typedef FrameLib_Vector<statFlatness>           FrameLib_Flatness;
typedef FrameLib_Vector<statRMS>                FrameLib_RMS;
typedef FrameLib_Vector<statCrest>              FrameLib_Crest;
typedef FrameLib_Vector<statArgMin>             FrameLib_VectorArgMin;
typedef FrameLib_Vector<statArgMax>             FrameLib_VectorArgMax;


#endif
