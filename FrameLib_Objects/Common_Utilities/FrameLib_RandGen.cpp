
#ifdef __APPLE__
#include <stdlib.h>
#elif defined (__linux__)
#include <stdlib.h>
#include <ctime>
#else
#include "windows.h"
#endif

#include "FrameLib_RandGen.h"

// Generate a Single Pseudo-random Unsigned Integer

uint32_t FrameLib_RandGen::randInt()
{
    return CMWC();
}

// Return an Unsigned 32 bit Integer

uint32_t FrameLib_RandGen::randInt(uint32_t n)
{
    uint32_t used = n;
    uint32_t i;
    
    used |= used >> 1;
    used |= used >> 2;
    used |= used >> 4;
    used |= used >> 8;
    used |= used >> 16;
    
    do
        i = randInt() & used;   // toss unused bits shortens search
    while (i > n);
    
    return i;
}

// Return a Signed 32 bit Integer in the Range [lo, hi]

int32_t FrameLib_RandGen::randInt(int32_t lo, int32_t hi)
{
    return lo + randInt(hi - lo);
}

// Return a 32 bit Random Double in the Range [0,1]

double FrameLib_RandGen::randDouble()
{
    return randInt() * 2.32830643653869628906e-10;
}

// Return a 32 bit Random Double in the Range [0, n]

double FrameLib_RandGen::randDouble(double n)
{
    return randDouble() * n;
}

// Return a 32 bit Random Double in the Range [lo, hi]

double FrameLib_RandGen::randDouble(double lo, double hi)
{
    return lo + randDouble() * (hi - lo);
}

// Gaussian Helper

void FrameLib_RandGen::randGaussians(double& x, double& y, double& R)
{
    x = 0.0;
    y = 0.0;
    R = 0.0;
    
    while (R >= 1.0 || R == 0.0)
    {
        x = randDouble(-1.0, 1.0);
        y = randDouble(-1.0, 1.0);
        R = (x * x) + (y * y);
    }
    
    R = sqrt((-2.0 * log(R)) / R);
}

// Return a 32 bit Random Double of Gaussian Distribution with given Mean / Deviation

double FrameLib_RandGen::randGaussian(double mean, double dev)
{
    double x, y, R;
    
    randGaussians(x, y, R);
    
    return (R * x) * dev + mean;
}

// Generate two independent gaussians (Mean 0 and Deviation 1)

void FrameLib_RandGen::randGaussians(double& x, double& y)
{
    double R;
    
    randGaussians(x, y, R);
    
    x *= R;
    y *= R;
}

// Methods Specific to the RNG Algorithm

// Basic Generator

inline uint32_t FrameLib_RandGen::CMWC()
{
    uint32_t i = mIncrement;
    uint32_t c = mCarry;
    uint32_t x;
    
    uint64_t t;
    
    i = (i + 1) & (cmwc_lag_size - 1);
    t = (uint64_t) cmwc_a_value * mState[i] + c;
    c = (t >> 32);
    x = (uint32_t) ((t + c) & 0xFFFFFFFF);
    
    if (x < c)
    {
        x++;
        c++;
    }
    
    mState[i] = (0xFFFFFFFE - x);
    mIncrement = i;
    mCarry = c;
    
    return mState[i];
}

// Initialise with Seed Values

void FrameLib_RandGen::initSeedCMWC(uint32_t *init)
{
    mIncrement = (cmwc_lag_size - 1);
    mCarry = 123;
    
    for (uint32_t i = 0; i < cmwc_lag_size; i++)
        mState[i] = init[i];
}

void FrameLib_RandGen::randSeedCMWC()
{
    uint32_t seeds[cmwc_lag_size];
    
#ifdef __APPLE__
    for (uint32_t i = 0; i < cmwc_lag_size; i++)
        seeds[i] = arc4random();
#elif defined (__linux__)
    srandom(time(nullptr));
    for (uint32_t i = 0; i < cmwc_lag_size; i++)
        seeds[i] = random();
#else
    HCRYPTPROV hProvider = 0;
    const DWORD dwLength = 4 * cmwc_lag_size;
    BYTE *pbBuffer = (BYTE *) seeds;
    
    if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
        return;
    
    CryptGenRandom(hProvider, dwLength, pbBuffer);
    CryptReleaseContext(hProvider, 0);
#endif
    initSeedCMWC(seeds);
}
