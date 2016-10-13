
#ifndef _FRAMELIB_RANDGEN_
#define _FRAMELIB_RANDGEN_

#include "FrameLib_Types.h"

class FrameLib_RandGen
{
    static const uint32_t CMWC_LAG_SIZE = 32;
    static const uint64_t CMWC_A_VALUE = 987655670LL;
    
    // Routines that are specific to the generator
    
    inline uint32_t CMWC();
    
    // Initialise with seed values
    
    void initCMWC(uint32_t *init);
    
    // Seed the random number generator randomly using OS specific routines
    
    void randSeedCMWC();
    
public:
    
    // Methods that are generator agnostic
    
    FrameLib_RandGen() { randSeed(); }
    
    void randSeed() { randSeedCMWC(); }
    
    // Initialise with seed values
    
    void init(uint32_t *init) { initCMWC(init); }
    
    // Generate a single pseudo-random unsigned integer
    
    uint32_t randInt();
        
    // Return an unsigned 32 bit integer  in the range [0, n]
    
    uint32_t randInt(uint32_t n);
    
    // Return an signed 32 bit integer in the range [lo, hi]
    
    int32_t randInt(int32_t lo, int32_t hi);
    
    // Return a 32 bit random double in the range [0,1]
    
    double randDouble();
    
    // Return a 32 bit random double in the range [0, n]
    
    double randDouble (double n);
    
    // Return a 32 bit random double in the range [lo, hi]
    
    double randDouble(double lo, double hi);

private:
    
    uint32_t mIncrement;
	uint32_t mCarry;
	uint32_t mSTATE[CMWC_LAG_SIZE];
};

#endif