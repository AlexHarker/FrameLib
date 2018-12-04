
#ifndef _FRAMELIB_RANDGEN_
#define _FRAMELIB_RANDGEN_

#include "FrameLib_Types.h"

class FrameLib_RandGen
{
    static const uint32_t CMWC_LAG_SIZE = 32;
    static const uint64_t CMWC_A_VALUE = 987655670LL;
    
public:
    
    FrameLib_RandGen()              { randSeed(); }
    
    // Seeding (specific / random values)
    
    void initSeed(uint32_t *init)   { initSeedCMWC(init); }
    void randSeed()                 { randSeedCMWC(); }
    
    // Generate a Single Pseudo-random Unsigned Integer (full range /  in the range [0, n] / in the range [lo, hi])
    
    uint32_t randInt();
    uint32_t randInt(uint32_t n);
    int32_t randInt(int32_t lo, int32_t hi);
    
    // Generate a 32 bit Random Double (in the range [0,1] / in the range [0, n] / in the range [lo, hi])
    
    double randDouble();
    double randDouble(double n);
    double randDouble(double lo, double hi);

    // Generate a 32 bit Random Double of Gaussian Distribution with given Mean / Deviation
    
    double randGaussian(double mean, double dev);
    
private:
    
    // Methods Specific to the RNG Algorithm
    
    // Basic Generator

    inline uint32_t CMWC();
    
    // Seeding (specific / OS-specific random values)
    
    void initSeedCMWC(uint32_t *init);    
    void randSeedCMWC();
    
    // State
    
    uint32_t mIncrement;
    uint32_t mCarry;
    uint32_t mSTATE[CMWC_LAG_SIZE];
};

#endif
