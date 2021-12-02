
#ifndef _FRAMELIB_RANDGEN_
#define _FRAMELIB_RANDGEN_

//#include "FrameLib_Types.h"

#include "RandomGenerator.hpp"

class FrameLib_RandGen : private random_generator<>
{
public:
        
    // Seeding (specific / random values)
    
    void initSeed(uint32_t *init)   { random_generator<>::seed(init); }
    void randSeed()                 { random_generator<>::rand_seed(); }
    
    // Generate a Single Pseudo-random Unsigned Integer (full range /  in the range [0, n] / in the range [lo, hi])
    
    uint32_t randInt()                              { return random_generator<>::rand_int(); }
    uint32_t randInt(uint32_t n)                    { return random_generator<>::rand_int(n); }
    int32_t randInt(int32_t lo, int32_t hi)         { return random_generator<>::rand_int(lo, hi); }
    
    // Generate a 32 bit Random Double (in the range [0,1] / in the range [0, n] / in the range [lo, hi])
    
    double randDouble()                             { return random_generator<>::rand_double(); }
    double randDouble(double n)                     { return random_generator<>::rand_double(n); }
    double randDouble(double lo, double hi)         { return random_generator<>::rand_double(lo, hi); }

    // Generate a 32 bit Random Double of Gaussian Distribution with given Mean / Deviation
    
    double randGaussian(double mean, double dev)    { return random_generator<>::rand_gaussian(mean, dev); }
    
    // Generate two independent gaussians (Mean 0 and Deviation 1)
    
    void randGaussians(double& x, double& y)         { return random_generator<>::rand_gaussians(x, y); }
};

#endif
