
#ifndef __APPLE__
#include "windows.h"
#endif

#include "FrameLib_RandGen.h"

// Return an signed 32 bit integer in the range [lo, hi]

int32_t FrameLib_RandGen::randInt(int32_t lo, int32_t hi)
{
    return lo + randInt(hi - lo);
}

// Return a 32 bit random double in the range [0,1]

double FrameLib_RandGen::randDouble()
{
    return randInt() * 2.32830643653869628906e-10;
}

// Return a 32 bit random double in the range [0, n]

double FrameLib_RandGen::randDouble (double n)
{
    return randDouble() * n;
}

// Return a 32 bit random double in the range [lo, hi]

double FrameLib_RandGen::randDouble(double lo, double hi)
{
    return lo + randDouble() * (hi - lo);
}

// Methods that are specific to the algorithm used

// Basic generator

inline uint32_t FrameLib_RandGen::CMWC()
{
    uint32_t i = mIncrement;
    uint32_t c = mCarry;
    uint32_t x;
    
    uint64_t t;
    
    i = (i + 1) & (CMWC_LAG_SIZE - 1);
    t = (uint64_t) CMWC_A_VALUE * mSTATE[i] + c;
    c = (t >> 32);
    //FIX - check this line and look elsewhere also...
    x = (uint32_t) ((t + c) & 0xFFFFFFFF);
    
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

void FrameLib_RandGen::initCMWC(uint32_t *init)
{
    mIncrement = (CMWC_LAG_SIZE - 1);
    mCarry = 123;
    
    for (uint32_t i = 0; i < CMWC_LAG_SIZE; i++)
        mSTATE[i] = init[i];
}

void FrameLib_RandGen::randSeedCMWC()
{
    uint32_t seeds[CMWC_LAG_SIZE];
    
#ifdef __APPLE__
    for (uint32_t i = 0; i < CMWC_LAG_SIZE; i++)
        seeds[i] = arc4random();
#else
    HCRYPTPROV hProvider = 0;
    const DWORD dwLength = 4 * CMWC_LAG_SIZE;
    BYTE *pbBuffer = (BYTE *) seeds;
    
    if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
        return;
    
    CryptGenRandom(hProvider, dwLength, pbBuffer);
    CryptReleaseContext(hProvider, 0);
#endif
    initCMWC(seeds);
}

// Generate a single pseudo-random unsigned integer

uint32_t FrameLib_RandGen::randInt()
{
    return CMWC();
}

// Return an unsigned 32 bit integer

inline uint32_t FrameLib_RandGen::randInt(uint32_t n)
{
    uint32_t used = n;
    uint32_t i;
    
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
