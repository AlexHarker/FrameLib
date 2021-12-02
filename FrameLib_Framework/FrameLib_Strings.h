
#ifndef FRAMELIB_STRINGS_H
#define FRAMELIB_STRINGS_H

#include <cstdio>
#include <cstring>

/**
 
 @defgroup Strings
 
 */


/**
 
 @class FrameLib_StringMaker
 
 @ingroup Strings

 @brief an simple struct to create string from numeric input
 
 */

template <int N = 32>
struct FrameLib_StringMaker
{
    FrameLib_StringMaker(size_t number)
    {
        snprintf(mChars, N, "%zu", number);
    }
    
    FrameLib_StringMaker(int number)
    {
        snprintf(mChars, N, "%d", number);
    }
    
    FrameLib_StringMaker(long number)
    {
        snprintf(mChars, N, "%ld", number);
    }
    
    FrameLib_StringMaker(double number, bool scientific = false)
    {
        if (scientific)
            snprintf(mChars, N, "%lg", number);
        else
            snprintf(mChars, N, "%lf", number);
    }
  
    FrameLib_StringMaker(const char *str, long number, long padding)
    {
        char formatStr[N];
        snprintf(formatStr, N, "%%s%%0%ldld", padding);
        snprintf(mChars, N, formatStr, str, number);
    }
    
    FrameLib_StringMaker(long number, long padding)
    {
        char formatStr[N];
        snprintf(formatStr, N, "%%0%ldld", padding);
        snprintf(mChars, N, formatStr, number);
    }
    
    operator const char *() const { return mChars; }
    
    char mChars[N];
};

#endif 
