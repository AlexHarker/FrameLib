

#ifndef _FRAMELIB_RANDGEN_
#define _FRAMELIB_RANDGEN_

#include "FrameLib_Types.h"


#define CMWC_LAG_SIZE 32
#define CMWC_A_VALUE 987655670LL


class FrameLib_RandGen
{
    
public:
    
    FrameLib_RandGen()
    {
        randSeed();
    }
    
private:
    
    // Routines that are speciifc to the generator
    
    inline FL_UInt32 CMWC()
    {
        FL_UInt32 i = mIncrement;
        FL_UInt32 c = mCarry;
        FL_UInt32 x;
        
        FL_UInt64 t;
        
        i = (i + 1) & (CMWC_LAG_SIZE - 1);
        t = (FL_UInt64) CMWC_A_VALUE * mSTATE[i] + c;
        c = (t >> 32);
        //FIX - check this line and look elsewhere also...
        x = (FL_UInt32) ((t + c) & 0xFFFFFFFF);
        
        if (x < c)
        {
            x++;
            c++;
        }
        
        mSTATE[i] = (0xFFFFFFFE - x);
        mIncrement = i;
        mCarry = c;
        
        return mSTATE[i];
    }
    
    // Initialise with seed values
    
    void initCMWC(FL_UInt32 *init)
    {
        mIncrement = (CMWC_LAG_SIZE - 1);
        mCarry = 123;
        
        for (long i = 0; i < CMWC_LAG_SIZE; i++)
            mSTATE[i] = init[i];
    }
    
public:

    // FIX - below is not quite true
    
    // Routines that are generator agnostic
    
    // Seed the random number generator randomly using OS specific routines
    
    void randSeed()
    {
        FL_UInt32 seeds[CMWC_LAG_SIZE];
        
#ifdef __APPLE__
        for (long i = 0; i < CMWC_LAG_SIZE; i++)
            seeds[i] = arc4random();
#else
        HCRYPTPROV hProvider = 0;
        const DWORD dwLength = 4 * CMWC_LAG_SIZE;
        BYTE *pbBuffer = (BYTE *) seed;
        
        if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
            return;
        
        CryptGenRandom(hProvider, dwLength, pbBuffer);
        CryptReleaseContext(hProvider, 0);
#endif
        
        initCMWC(seeds);
    }
    
    // Initialise with seed values
    
    void init(FL_UInt32 *init)
    {
        initCMWC(init);
    }
    
    // Generate a single pseudo-random unsigned integer
    
    inline FL_UInt32 randInt()
    {
        return CMWC();
    }
    
#define CONVERT_DOUBLE 2.32830643653869628906e-10
    
    // Return an unsigned 32 bit integer
    
    inline FL_UInt32 randInt(FL_UInt32 n)
    {
        FL_UInt32 used = n;
        FL_UInt32 i;
        
        used |= used >> 1;
        used |= used >> 2;
        used |= used >> 4;
        used |= used >> 8;
        used |= used >> 16;
        
        do
            i = randInt() & used;			// toss unused bits shortens search
        while (i > n);
        
        return i;
    }
    
    // Return an signed 32 bit integer in the range [lo, hi]
    
    inline FL_SInt32 randInt(FL_SInt32 lo, FL_SInt32 hi)
    {
        return lo + randInt(hi - lo);
    }
    
    
    // Return a 32 bit random double in the range [0,1]
    
    inline double randDouble()
    {
        return randInt() * CONVERT_DOUBLE;
    }
    
    
    // Return a 32 bit random double in the range [0, n]
    
    inline double randDouble (double n)
    {	
        return randDouble() * n;
    }
    
    
    // Return a 32 bit random double in the range [lo, hi]
    
    inline double randDouble(double lo, double hi)
    {	
        return lo + randDouble() * (hi - lo);
    }
    
private:
    
    FL_UInt32 mIncrement;
	FL_UInt32 mCarry;
	FL_UInt32 mSTATE[CMWC_LAG_SIZE];
};

#endif