
#ifndef FRAMELIB_VECTOR_OBJECTS_H
#define FRAMELIB_VECTOR_OBJECTS_H

#include "FrameLib_Vector_Template.h"
#include "../../FrameLib_Dependencies/Statistics.hpp"

// Info specialisations

template<> inline const char *FrameLib_Vector<statLength, true>::getOpString()          { return "length"; }
template<> inline const char *FrameLib_Vector<statMin>::getOpString()                   { return "minimum value"; }
template<> inline const char *FrameLib_Vector<statMax>::getOpString()                   { return "maximum value"; }
template<> inline const char *FrameLib_Vector<statSum>::getOpString()                   { return "sum"; }
template<> inline const char *FrameLib_Vector<statProduct>::getOpString()               { return "product"; }
template<> inline const char *FrameLib_Vector<statMean>::getOpString()                  { return "mean"; }
template<> inline const char *FrameLib_Vector<statGeometricMean>::getOpString()         { return "geometric mean"; }
template<> inline const char *FrameLib_Vector<statStandardDeviation>::getOpString()     { return "standard deviation"; }
template<> inline const char *FrameLib_Vector<statCentroid>::getOpString()              { return "centroid"; }
template<> inline const char *FrameLib_Vector<statSpread>::getOpString()                { return "spread"; }
template<> inline const char *FrameLib_Vector<statSkewness>::getOpString()              { return "skewness"; }
template<> inline const char *FrameLib_Vector<statKurtosis>::getOpString()              { return "kurtosis"; }
template<> inline const char *FrameLib_Vector<statFlatness>::getOpString()              { return "flatness"; }
template<> inline const char *FrameLib_Vector<statRMS>::getOpString()                   { return "rms"; }
template<> inline const char *FrameLib_Vector<statCrest>::getOpString()                 { return "crest factor"; }
template<> inline const char *FrameLib_Vector<statMinPosition>::getOpString()           { return "index of the minimum value"; }
template<> inline const char *FrameLib_Vector<statMaxPosition>::getOpString()           { return "index of the maximum value"; }

// Type definitions

typedef FrameLib_Vector<statLength, true>       FrameLib_Length;
typedef FrameLib_Vector<statMin>                FrameLib_FrameMin;
typedef FrameLib_Vector<statMax>                FrameLib_FrameMax;
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
typedef FrameLib_Vector<statMinPosition>        FrameLib_FrameMinPosition;
typedef FrameLib_Vector<statMaxPosition>        FrameLib_FrameMaxPosition;


#endif
