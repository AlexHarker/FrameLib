
#ifndef FRAMELIB_IO_HELPERS_H
#define FRAMELIB_IO_HELPERS_H

#include "../../FrameLib_Dependencies/TableReader.hpp"

struct FrameLib_IO_Helper
{
    template <class T>
    static void interpolate(T fetch, double *output, unsigned long size, double offset, InterpType interpType)
    {
        for (unsigned long i = 0; i < size; i++, offset += 1.0)
            output[i] = offset;
        
        table_read(fetch, output, output, size, 1.0, interpType);
    }

    template <class T>
    static void interpolate_zeropad(T fetch, double *output, unsigned long size, double offset, InterpType interpType)
    {
        interpolate(table_fetcher_zeropad<T>(fetch), output, size, offset, interpType);
    }
};

#endif 
