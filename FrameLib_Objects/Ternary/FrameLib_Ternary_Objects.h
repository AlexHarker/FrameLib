
#ifndef FRAMELIB_TERNARY_OBJECTS_H
#define FRAMELIB_TERNARY_OBJECTS_H

#include "FrameLib_Ternary_Template.h"

// Specification and signature taken from C++17 std::clamp, more or less
// If v compares less than lo, returns lo; otherwise if hi compares less than v, returns hi; otherwise returns v. Uses operator< to compare the values.

template<class T>  T clip(T v,T lo, T hi)
{
    return v < lo? lo : (v > hi? hi : v);
}

// Wrap and fold are taken from PD's cyclone pong external
// https://github.com/porres/pd-cyclone/blob/master/cyclone_src/binaries/signal/pong.c

template<class T> T wrap(T v, T lo, T hi)
{
    T returnval;
    T range = hi - lo;
    if(v < hi && v >= lo)
        return v;
    
    if (hi == lo)
        return lo;
    
    if(v < lo)
    {
        returnval = v;
        while (returnval < lo)
            returnval += range;
    }
    else
        returnval = std::fmod(v-lo,range) + lo;
    
    return returnval;
}

template<class T> T fold(T v, T lo, T hi)
{
    T returnval;
    T range = hi - lo;
    if(v < hi && v >= lo)
        return v;
    
    if (hi == lo)
        return lo;
    
    if(v < lo)
    {
        T diff = lo - v;
        unsigned long mag = static_cast<unsigned long>(diff/range); //case where v is more than a range away from lo
        if(mag % 2 == 0) // even number of ranges away => counting up from lo
        {
            diff = diff - (range * mag);
            returnval = diff + lo;
        }
        else            // odd number of ranges away => counting down from hi
        {
            diff = diff - (range * mag);
            returnval = hi - diff;
        }
    }
    else // v > hi
    {
        T diff = v - hi;
        unsigned long mag = static_cast<unsigned long>(diff/range);
        if(mag % 2 == 0) // even number of ranges away => counting up from lo
        {
            diff = diff - (range * mag);
            returnval = hi - diff;
        }
        else            // odd number of ranges away => counting down from hi
        {
            diff = diff - (range * mag);
            returnval = diff + lo;
        }
    }
    return returnval; 
}

typedef FrameLib_Ternary<clip<double> >         FrameLib_Clip;
typedef FrameLib_Ternary<wrap<double> >         FrameLib_Wrap;
typedef FrameLib_Ternary<fold<double> >         FrameLib_Fold; 

#endif 

