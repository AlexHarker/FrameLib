
#ifndef FRAMELIB_VECTOR_OBJECTS_H
#define FRAMELIB_VECTOR_OBJECTS_H

#include "FrameLib_Vector_Template.h"
#include "../../FrameLib_Dependencies/Statistics.hpp"

// Info specialisations

template<> inline const char *FrameLib_Vector<stat_length, kAlwaysCalculate>::getOpString()     { return "length"; }
template<> inline const char *FrameLib_Vector<stat_min, kDefaultPosInf>::getOpString()          { return "minimum value"; }
template<> inline const char *FrameLib_Vector<stat_max, kDefaultNegInf>::getOpString()          { return "maximum value"; }
template<> inline const char *FrameLib_Vector<stat_sum>::getOpString()                          { return "sum"; }
template<> inline const char *FrameLib_Vector<stat_product, kDefaultOne>::getOpString()         { return "product"; }
template<> inline const char *FrameLib_Vector<stat_mean>::getOpString()                         { return "mean"; }
template<> inline const char *FrameLib_Vector<stat_geometric_mean>::getOpString()               { return "geometric mean"; }
template<> inline const char *FrameLib_Vector<stat_standard_deviation>::getOpString()           { return "standard deviation"; }
template<> inline const char *FrameLib_Vector<stat_centroid>::getOpString()                     { return "centroid"; }
template<> inline const char *FrameLib_Vector<stat_spread>::getOpString()                       { return "spread"; }
template<> inline const char *FrameLib_Vector<stat_skewness>::getOpString()                     { return "skewness"; }
template<> inline const char *FrameLib_Vector<stat_kurtosis, kDefaultOne>::getOpString()        { return "kurtosis"; }
template<> inline const char *FrameLib_Vector<stat_flatness, kDefaultOne>::getOpString()        { return "flatness (as a ratio)"; }
template<> inline const char *FrameLib_Vector<stat_rms>::getOpString()                          { return "rms"; }
template<> inline const char *FrameLib_Vector<stat_crest, kDefaultOne>::getOpString()           { return "crest factor"; }
template<> inline const char *FrameLib_Vector<stat_min_position>::getOpString()                 { return "index of the minimum value"; }
template<> inline const char *FrameLib_Vector<stat_max_position>::getOpString()                 { return "index of the maximum value"; }

// Type definitions

typedef FrameLib_Vector<stat_length, kAlwaysCalculate>      FrameLib_Length;
typedef FrameLib_Vector<stat_min, kDefaultPosInf>           FrameLib_FrameMin;
typedef FrameLib_Vector<stat_max, kDefaultNegInf>           FrameLib_FrameMax;
typedef FrameLib_Vector<stat_sum>                           FrameLib_Sum;
typedef FrameLib_Vector<stat_product, kDefaultOne>          FrameLib_Product;
typedef FrameLib_Vector<stat_mean>                          FrameLib_Mean;
typedef FrameLib_Vector<stat_geometric_mean>                FrameLib_GeometricMean;
typedef FrameLib_Vector<stat_standard_deviation>            FrameLib_StandardDeviation;
typedef FrameLib_Vector<stat_centroid>                      FrameLib_Centroid;
typedef FrameLib_Vector<stat_spread>                        FrameLib_Spread;
typedef FrameLib_Vector<stat_skewness>                      FrameLib_Skewness;
typedef FrameLib_Vector<stat_kurtosis, kDefaultOne>         FrameLib_Kurtosis;
typedef FrameLib_Vector<stat_flatness, kDefaultOne>         FrameLib_Flatness;
typedef FrameLib_Vector<stat_rms>                           FrameLib_RMS;
typedef FrameLib_Vector<stat_crest, kDefaultOne>            FrameLib_Crest;
typedef FrameLib_Vector<stat_min_position>                  FrameLib_FrameMinPosition;
typedef FrameLib_Vector<stat_max_position>                  FrameLib_FrameMaxPosition;

#endif
