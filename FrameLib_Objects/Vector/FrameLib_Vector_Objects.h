
#ifndef FRAMELIB_VECTOR_OBJECTS_H
#define FRAMELIB_VECTOR_OBJECTS_H

#include "FrameLib_Vector_Template.h"
#include "../../FrameLib_Dependencies/Statistics.hpp"

// Info specialisations

template<> inline const char *FrameLib_Vector<statLength, kAlwaysCalculate>::getOpString()      { return "length"; }
template<> inline const char *FrameLib_Vector<statMin, kDefaultPosInf>::getOpString()           { return "minimum value"; }
template<> inline const char *FrameLib_Vector<statMax, kDefaultNegInf>::getOpString()           { return "maximum value"; }
template<> inline const char *FrameLib_Vector<statSum>::getOpString()                           { return "sum"; }
template<> inline const char *FrameLib_Vector<statProduct, kDefaultOne>::getOpString()          { return "product"; }
template<> inline const char *FrameLib_Vector<statMean>::getOpString()                          { return "mean"; }
template<> inline const char *FrameLib_Vector<statGeometricMean>::getOpString()                 { return "geometric mean"; }
template<> inline const char *FrameLib_Vector<statStandardDeviation>::getOpString()             { return "standard deviation"; }
template<> inline const char *FrameLib_Vector<statCentroid>::getOpString()                      { return "centroid"; }
template<> inline const char *FrameLib_Vector<statSpread>::getOpString()                        { return "spread"; }
template<> inline const char *FrameLib_Vector<statSkewness>::getOpString()                      { return "skewness"; }
template<> inline const char *FrameLib_Vector<statKurtosis, kDefaultOne>::getOpString()         { return "kurtosis"; }
template<> inline const char *FrameLib_Vector<statFlatness, kDefaultOne>::getOpString()         { return "flatness"; }
template<> inline const char *FrameLib_Vector<statRMS>::getOpString()                           { return "rms"; }
template<> inline const char *FrameLib_Vector<statCrest, kDefaultOne>::getOpString()            { return "crest factor"; }
template<> inline const char *FrameLib_Vector<statMinPosition>::getOpString()                   { return "index of the minimum value"; }
template<> inline const char *FrameLib_Vector<statMaxPosition>::getOpString()                   { return "index of the maximum value"; }

// Type definitions

typedef FrameLib_Vector<statLength, kAlwaysCalculate>       FrameLib_Length;
typedef FrameLib_Vector<statMin, kDefaultPosInf>            FrameLib_FrameMin;
typedef FrameLib_Vector<statMax, kDefaultNegInf>            FrameLib_FrameMax;
typedef FrameLib_Vector<statSum>                            FrameLib_Sum;
typedef FrameLib_Vector<statProduct, kDefaultOne>           FrameLib_Product;
typedef FrameLib_Vector<statMean>                           FrameLib_Mean;
typedef FrameLib_Vector<statGeometricMean>                  FrameLib_GeometricMean;
typedef FrameLib_Vector<statStandardDeviation>              FrameLib_StandardDeviation;
typedef FrameLib_Vector<statCentroid>                       FrameLib_Centroid;
typedef FrameLib_Vector<statSpread>                         FrameLib_Spread;
typedef FrameLib_Vector<statSkewness>                       FrameLib_Skewness;
typedef FrameLib_Vector<statKurtosis, kDefaultOne>          FrameLib_Kurtosis;
typedef FrameLib_Vector<statFlatness, kDefaultOne>          FrameLib_Flatness;
typedef FrameLib_Vector<statRMS>                            FrameLib_RMS;
typedef FrameLib_Vector<statCrest, kDefaultOne>             FrameLib_Crest;
typedef FrameLib_Vector<statMinPosition>                    FrameLib_FrameMinPosition;
typedef FrameLib_Vector<statMaxPosition>                    FrameLib_FrameMaxPosition;

#endif
