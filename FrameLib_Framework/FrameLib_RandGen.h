
#ifndef _FRAMELIB_RANDGEN_
#define _FRAMELIB_RANDGEN_

#include "FrameLib_Types.h"

class FrameLib_RandGen
{
    static const FL_UInt32 CMWC_LAG_SIZE = 32;
    static const FL_UInt64 CMWC_A_VALUE = 987655670LL;
    
    // Routines that are specific to the generator
    
    inline FL_UInt32 CMWC();
    
    // Initialise with seed values
    
    void initCMWC(FL_UInt32 *init);
    
    // Seed the random number generator randomly using OS specific routines
    
    void randSeedCMWC();
    
public:
    
    // Methods that are generator agnostic
    
    FrameLib_RandGen() { randSeed(); }
    
    void randSeed() { randSeedCMWC(); }
    
    // Initialise with seed values
    
    void init(FL_UInt32 *init) { initCMWC(init); }
    
    // Generate a single pseudo-random unsigned integer
    
    FL_UInt32 randInt();
        
    // Return an unsigned 32 bit integer  in the range [0, n]
    
    FL_UInt32 randInt(FL_UInt32 n);
    
    // Return an signed 32 bit integer in the range [lo, hi]
    
    FL_SInt32 randInt(FL_SInt32 lo, FL_SInt32 hi);
    
    // Return a 32 bit random double in the range [0,1]
    
    double randDouble();
    
    // Return a 32 bit random double in the range [0, n]
    
    double randDouble (double n);
    
    // Return a 32 bit random double in the range [lo, hi]
    
    double randDouble(double lo, double hi);

private:
    
    FL_UInt32 mIncrement;
	FL_UInt32 mCarry;
	FL_UInt32 mSTATE[CMWC_LAG_SIZE];
};

#endif