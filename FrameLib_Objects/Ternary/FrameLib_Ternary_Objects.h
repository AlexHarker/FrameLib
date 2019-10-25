
#ifndef FRAMELIB_TERNARY_OBJECTS_H
#define FRAMELIB_TERNARY_OBJECTS_H

#include "FrameLib_Ternary_Template.h"

namespace FrameLib_Ternary_Ops
{
    // Specification/signature taken from C++17 std::clamp, (although that specifies use of only < rather than < and >)
    // If v compares less than lo, returns lo; otherwise if hi compares less than v, returns hi; otherwise returns v.

    template<class T>  T clip(T v,T lo, T hi)
    {
        if (lo > hi)
            std::swap(lo, hi);
        
        return v < lo ? lo : (v > hi ? hi : v);
    }

    // Wrap is modified from PD's cyclone pong external
    // https://github.com/porres/pd-cyclone/blob/master/cyclone_objects/binaries/audio/pong.c

    template <class T> T wrap(T v, T lo, T hi)
    {
        if (lo > hi)
            std::swap(lo, hi);
        
        T returnVal;
        T range = hi - lo;
        
        if (v < hi && v >= lo)
            return v;
        
        if (hi == lo)
            return lo;
        
        if (v < lo)
        {
            returnVal = v;
            while (returnVal < lo)
                returnVal += range;
        }
        else
            returnVal = std::fmod(v - lo, range) + lo;
        
        return returnVal;
    }

    // Fold is modified from PD's cyclone pong external
    // https://github.com/porres/pd-cyclone/blob/master/cyclone_objects/binaries/audio/pong.c

    template <class T> T fold(T v, T lo, T hi)
    {
        if (lo > hi)
            std::swap(lo, hi);
        
        T range = hi - lo;
        
        if (v <= hi && v >= lo)
            return v;
        
        if (hi == lo)
            return lo;
        
        bool below = (v < lo);
        T diff = below ? lo - v : v - hi;
        unsigned long mag = static_cast<unsigned long>(diff/range);
        
        diff = diff - (range * mag);
        
        if ((mag % 2 == 0) ^ below)
            return hi - diff;
        else
            return lo + diff;
    }
};

template<>
inline const char* FrameLib_Ternary<FrameLib_Ternary_Ops::clip<double>>::getDescriptionString()
{
    return "Clips the incoming frame to the range delimited by the two parameters";
}

template<>
inline const char* FrameLib_Ternary<FrameLib_Ternary_Ops::wrap<double>>::getDescriptionString()
{
    return "Wraps the incoming frame into the range delimited by the two parameters";
}

template<>
inline const char* FrameLib_Ternary<FrameLib_Ternary_Ops::fold<double>>::getDescriptionString()
{
    return "Folds the incoming frame into the range delimited by the two parameters";
}

typedef FrameLib_Ternary<FrameLib_Ternary_Ops::clip<double>> FrameLib_Clip;
typedef FrameLib_Ternary<FrameLib_Ternary_Ops::wrap<double>> FrameLib_Wrap;
typedef FrameLib_Ternary<FrameLib_Ternary_Ops::fold<double>> FrameLib_Fold;

#endif 

