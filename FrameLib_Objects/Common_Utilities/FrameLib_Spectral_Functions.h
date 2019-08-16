
#ifndef FRAMELIB_SPECTRAL_FUNCTIONS_H
#define FRAMELIB_SPECTRAL_FUNCTIONS_H

static unsigned long ilog2(unsigned long value)
{
    unsigned long bitShift = value;
    unsigned long bitCount = 0;
    
    while (bitShift)
    {
        bitShift >>= 1U;
        bitCount++;
    }
    
    if (bitCount && value == 1U << (bitCount - 1U))
        return bitCount - 1U;
    else
        return bitCount;
};

#endif
