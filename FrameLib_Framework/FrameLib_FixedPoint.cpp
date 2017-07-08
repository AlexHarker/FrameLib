
#include "FrameLib_FixedPoint.h"

// Utility

// Bit Manipulation

uint64_t lo32Bits(uint64_t a)                        { return a & 0xFFFFFFFF; }
uint64_t hi32Bits(uint64_t a)                        { return a >> 0x20; }
uint64_t joinBits(uint64_t hi, uint64_t lo)          { return (hi << 0x20) | hi32Bits(lo); }
uint64_t loToHiBits(uint64_t a)                      { return (a << 0x20); }

bool checkHighBit(uint64_t a)                        { return (a & 0x8000000000000000ULL) ? true : false; }

// Single 64 bit add/subtract with carry bit

bool addWithCarry(uint64_t *result, const uint64_t& a, const uint64_t& b)
{
    *result = a + b;
    return (*result < b);
}

bool subWithCarry(uint64_t *result, const uint64_t& a, const uint64_t& b)
{
    *result = a - b;
    return (*result > a);
}

// ************************************************************************************** //

FL_SP qMul(const FL_SP& a, const uint64_t &intVal, const uint64_t &fracVal)
{
    // Split both into 6/4 x 32 bits stored within 64 bit integers
    
    const uint64_t a1 = hi32Bits(a.mInt);
    const uint64_t a2 = lo32Bits(a.mInt);
    const uint64_t a3 = hi32Bits(a.mFracHi);
    const uint64_t a4 = lo32Bits(a.mFracHi);
    const uint64_t a5 = hi32Bits(a.mFracLo);
    const uint64_t a6 = lo32Bits(a.mFracLo);
    
    const uint64_t b1 = hi32Bits(intVal);
    const uint64_t b2 = lo32Bits(intVal);
    const uint64_t b3 = hi32Bits(fracVal);
    const uint64_t b4 = lo32Bits(fracVal);
    
    const uint64_t a1b1 = a1 * b1;
    const uint64_t a1b2 = a1 * b2;
    const uint64_t a1b3 = a1 * b3;
    const uint64_t a1b4 = a1 * b4;
    
    const uint64_t a2b1 = a2 * b1;
    const uint64_t a2b2 = a2 * b2;
    const uint64_t a2b3 = a2 * b3;
    const uint64_t a2b4 = a2 * b4;
    
    const uint64_t a3b1 = a3 * b1;
    const uint64_t a3b2 = a3 * b2;
    const uint64_t a3b3 = a3 * b3;
    const uint64_t a3b4 = a3 * b4;
    
    const uint64_t a4b1 = a4 * b1;
    const uint64_t a4b2 = a4 * b2;
    const uint64_t a4b3 = a4 * b3;
    const uint64_t a4b4 = a4 * b4;
    
    const uint64_t a5b1 = a5 * b1;
    const uint64_t a5b2 = a5 * b2;
    const uint64_t a5b3 = a5 * b3;
    const uint64_t a5b4 = a5 * b4;
    
    const uint64_t a6b1 = a6 * b1;
    const uint64_t a6b2 = a6 * b2;
    const uint64_t a6b3 = a6 * b3;
    const uint64_t a6b4 = a6 * b4;
    
    // Lowest carry bits
    
    uint64_t t1 = 0ULL;
    unsigned int c1 = addWithCarry(&t1, a6b4, loToHiBits(a6b3)) ? 1U : 0U;
    c1 = addWithCarry(&t1, t1, loToHiBits(a5b4)) ? ++c1: c1;
    
    // Round
    
    c1 = checkHighBit(t1) ? ++c1 : c1;
    
    // Sum the lo fractional part
    
    uint64_t lo = 0ULL;
    unsigned int c2 = addWithCarry(&lo, a6b2, a5b3) ? 1U : 0U;
    c2 = addWithCarry(&lo, lo, a4b4) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, joinBits(a6b1, a6b3)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, joinBits(a5b2, a5b4)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, loToHiBits(a4b3)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, loToHiBits(a3b4) | c1) ? ++c2: c2;
    
    // Sum the hi fractional part
    
    uint64_t md = 0ULL;
    unsigned int c3 = addWithCarry(&md, a2b4, a4b2) ? 1U : 0U;
    c3 = addWithCarry(&md, md, a3b3) ? ++c3: c3;
    c3 = addWithCarry(&md, md, a5b1) ? ++c3: c3;
    c3 = addWithCarry(&md, md, joinBits(a1b4, a4b3)) ? ++c3: c3;
    c3 = addWithCarry(&md, md, joinBits(a4b1, a3b4)) ? ++c3: c3;
    c3 = addWithCarry(&md, md, joinBits(a2b3, a6b1)) ? ++c3: c3;
    c3 = addWithCarry(&md, md, joinBits(a3b2, a5b2)) ? ++c3: c3;
    c3 = addWithCarry(&md, md, c2) ? ++c3: c3;
    
    // Sum the integer part
    
    uint64_t hi = 0ULL;
    unsigned int c4 = addWithCarry(&hi, a2b2, a1b3) ? 1U : 0U;
    c4 = addWithCarry(&hi, hi, a3b1) ? ++c4: c4;
    c4 = addWithCarry(&hi, hi, joinBits(a1b2, a1b4)) ? ++c4: c4;
    c4 = addWithCarry(&hi, hi, joinBits(a2b1, a4b1)) ? ++c4: c4;
    c4 = addWithCarry(&hi, hi, hi32Bits(a2b3) + hi32Bits(a3b2)) ? ++c4: c4;
    c4 = addWithCarry(&hi, hi, c3) ? ++c4: c4;
    
    // Do overflow
    
    t1 = (a1b1 | hi32Bits(a1b2) | hi32Bits(a2b1) | c4) ? FL_Limits<uint64_t>::largest() : 0ULL;
    
    return FL_SP(hi | t1, md | t1, lo | t1);
}

FL_SP operator * (const FL_SP& a, const FL_SP& b)
{
    // Split both into 6 x 32 bits stored within 64 bit integers
    
    const uint64_t a1 = hi32Bits(a.mInt);
    const uint64_t a2 = lo32Bits(a.mInt);
    const uint64_t a3 = hi32Bits(a.mFracHi);
    const uint64_t a4 = lo32Bits(a.mFracHi);
    const uint64_t a5 = hi32Bits(a.mFracLo);
    const uint64_t a6 = lo32Bits(a.mFracLo);
    
    const uint64_t b1 = hi32Bits(b.mInt);
    const uint64_t b2 = lo32Bits(b.mInt);
    const uint64_t b3 = hi32Bits(b.mFracHi);
    const uint64_t b4 = lo32Bits(b.mFracHi);
    const uint64_t b5 = hi32Bits(b.mFracLo);
    const uint64_t b6 = lo32Bits(b.mFracLo);
    
    const uint64_t a1b1 = a1 * b1;
    const uint64_t a1b2 = a1 * b2;
    const uint64_t a1b3 = a1 * b3;
    const uint64_t a1b4 = a1 * b4;
    const uint64_t a1b5 = a1 * b5;
    const uint64_t a1b6 = a1 * b6;
    
    const uint64_t a2b1 = a2 * b1;
    const uint64_t a2b2 = a2 * b2;
    const uint64_t a2b3 = a2 * b3;
    const uint64_t a2b4 = a2 * b4;
    const uint64_t a2b5 = a2 * b5;
    const uint64_t a2b6 = a2 * b6;
    
    const uint64_t a3b1 = a3 * b1;
    const uint64_t a3b2 = a3 * b2;
    const uint64_t a3b3 = a3 * b3;
    const uint64_t a3b4 = a3 * b4;
    const uint64_t a3b5 = a3 * b5;
    const uint64_t a3b6 = a3 * b6;
    
    const uint64_t a4b1 = a4 * b1;
    const uint64_t a4b2 = a4 * b2;
    const uint64_t a4b3 = a4 * b3;
    const uint64_t a4b4 = a4 * b4;
    const uint64_t a4b5 = a4 * b5;
    const uint64_t a4b6 = a4 * b6;
    
    const uint64_t a5b1 = a5 * b1;
    const uint64_t a5b2 = a5 * b2;
    const uint64_t a5b3 = a5 * b3;
    const uint64_t a5b4 = a5 * b4;
    const uint64_t a5b5 = a5 * b5;
    const uint64_t a5b6 = a5 * b6;
    
    const uint64_t a6b1 = a6 * b1;
    const uint64_t a6b2 = a6 * b2;
    const uint64_t a6b3 = a6 * b3;
    const uint64_t a6b4 = a6 * b4;
    const uint64_t a6b5 = a6 * b5;
    const uint64_t a6b6 = a6 * b6;
    
    // Sub carry bits
    
    uint64_t t1 = 0ULL;
    unsigned int c0 = addWithCarry(&t1, a6b6, loToHiBits(a5b6)) ? 1U : 0U;
    c0 = addWithCarry(&t1, t1, loToHiBits(a6b5)) ? ++c0: c0;
    
    // Lowest carry bits
    
    t1 = 0ULL;
    unsigned int c1 = addWithCarry(&t1, a4b6, a6b4) ? 1U : 0U;
    c1 = addWithCarry(&t1, t1, a5b5) ? ++c1: c1;
    c1 = addWithCarry(&t1, t1, joinBits(a3b6, a5b6)) ? ++c1: c1;
    c1 = addWithCarry(&t1, t1, joinBits(a6b3, a6b5)) ? ++c1: c1;
    c1 = addWithCarry(&t1, t1, loToHiBits(a4b5)) ? ++c1: c1;
    c1 = addWithCarry(&t1, t1, loToHiBits(a5b4) | c0) ? ++c1: c1;
    
    // Round
    
    c1 = checkHighBit(t1) ? ++c1 : c1;
    
    // Sum the lo fractional part
    
    uint64_t lo = 0ULL;
    unsigned int c2 = addWithCarry(&lo, a2b6, a6b2) ? 1U : 0U;
    c2 = addWithCarry(&lo, lo, a3b5) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, a5b3) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, a4b4) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, joinBits(a1b6, a3b6)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, joinBits(a6b1, a6b3)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, joinBits(a2b5, a4b5)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, joinBits(a5b2, a5b4)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, loToHiBits(a4b3)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, loToHiBits(a3b4) | c1) ? ++c2: c2;
    
    // Sum the hi fractional part
    
    uint64_t md = 0ULL;
    unsigned int c3 = addWithCarry(&md, a2b4, a4b2) ? 1U : 0U;
    c3 = addWithCarry(&md, md, a3b3) ? ++c3: c3;
    c3 = addWithCarry(&md, md, a1b5) ? ++c3: c3;
    c3 = addWithCarry(&md, md, a5b1) ? ++c3: c3;
    c3 = addWithCarry(&md, md, joinBits(a1b4, a4b3)) ? ++c3: c3;
    c3 = addWithCarry(&md, md, joinBits(a4b1, a3b4)) ? ++c3: c3;
    c3 = addWithCarry(&md, md, joinBits(a2b3, a1b6)) ? ++c3: c3;
    c3 = addWithCarry(&md, md, joinBits(a3b2, a6b1)) ? ++c3: c3;
    c3 = addWithCarry(&md, md, hi32Bits(a2b5) + hi32Bits(a5b2)) ? ++c3: c3;
    c3 = addWithCarry(&md, md, c2) ? ++c3: c3;
    
    // Sum the integer part
    
    uint64_t hi = 0ULL;
    unsigned int c4 = addWithCarry(&hi, a2b2, a1b3) ? 1U : 0U;
    c4 = addWithCarry(&hi, hi, a3b1) ? ++c4: c4;
    c4 = addWithCarry(&hi, hi, joinBits(a1b2, a1b4)) ? ++c4: c4;
    c4 = addWithCarry(&hi, hi, joinBits(a2b1, a4b1)) ? ++c4: c4;
    c4 = addWithCarry(&hi, hi, hi32Bits(a2b3) + hi32Bits(a3b2)) ? ++c4: c4;
    c4 = addWithCarry(&hi, hi, c3) ? ++c4: c4;
    
    // Do overflow
    
    t1 = (a1b1 | hi32Bits(a1b2) | hi32Bits(a2b1) | c4) ? FL_Limits<uint64_t>::largest() : 0ULL;
    
    return FL_SP(hi | t1, md | t1, lo | t1);
}

FL_SP twoMinus(const FL_SP& b)
{
    uint64_t hi = 2 - b.mInt;
    uint64_t md = -b.mFracHi;
    uint64_t lo = -b.mFracLo;
    
    // N.B. - must be able to double carry!
    
    if (b.mFracHi | b.mFracLo)
        --hi;
    
    if (b.mFracLo)
        --md;
    
    return FL_SP(hi, md, lo);
}

// ************************************************************************************** //

// FL_FP

// This class provides a very high precision fixed point format for dealing with time

FL_FP::FL_FP(const double& val)
{
    double absVal = fabs(val);
    
    if (18446744073709551616.0 <= absVal)
        mInt = mFrac = FL_Limits<uint64_t>::largest();
    else
    {
        mInt = absVal;
        mFrac = round((absVal - floor(absVal)) * 18446744073709551616.0);
    }
}

FL_FP::FL_FP(const FL_SP& val) : mInt(val.intVal()), mFrac(val.fracHiVal())
{
    // Complete rounding
    
    if (checkHighBit(val.fracLoVal()))
        *this += FL_FP(0, 1);
}

FL_FP operator + (const FL_FP& a, const FL_FP& b)
{
    uint64_t lo = 0ULL;
    uint64_t hi = a.mInt + b.mInt;
    hi = addWithCarry(&lo, a.mFrac, b.mFrac) ? ++hi : hi;
    
    return FL_FP(hi, lo);
}

FL_FP operator - (const FL_FP& a, const FL_FP& b)
{
    uint64_t lo = 0ULL;
    uint64_t hi = a.mInt - b.mInt;
    hi = subWithCarry(&lo, a.mFrac, b.mFrac) ? --hi : hi;
    
    return FL_FP(hi, lo);
}

FL_FP operator * (const FL_FP& a, const FL_FP& b)
{
    // Split both into 4 x 32 bits stored within 64 bit integers
    
    const uint64_t a1 = hi32Bits(a.mInt);
    const uint64_t a2 = lo32Bits(a.mInt);
    const uint64_t a3 = hi32Bits(a.mFrac);
    const uint64_t a4 = lo32Bits(a.mFrac);
    
    const uint64_t b1 = hi32Bits(b.mInt);
    const uint64_t b2 = lo32Bits(b.mInt);
    const uint64_t b3 = hi32Bits(b.mFrac);
    const uint64_t b4 = lo32Bits(b.mFrac);
    
    const uint64_t a1b1 = a1 * b1;
    const uint64_t a1b2 = a1 * b2;
    const uint64_t a1b3 = a1 * b3;
    const uint64_t a1b4 = a1 * b4;
    
    const uint64_t a2b1 = a2 * b1;
    const uint64_t a2b2 = a2 * b2;
    const uint64_t a2b3 = a2 * b3;
    const uint64_t a2b4 = a2 * b4;
    
    const uint64_t a3b1 = a3 * b1;
    const uint64_t a3b2 = a3 * b2;
    const uint64_t a3b3 = a3 * b3;
    const uint64_t a3b4 = a3 * b4;
    
    const uint64_t a4b1 = a4 * b1;
    const uint64_t a4b2 = a4 * b2;
    const uint64_t a4b3 = a4 * b3;
    const uint64_t a4b4 = a4 * b4;
    
    // Lowest carry bits
    
    uint64_t t1 = 0ULL;
    unsigned int c1 = addWithCarry(&t1, a4b4, loToHiBits(a4b3)) ? 1U : 0U;
    c1 = addWithCarry(&t1, t1, loToHiBits(a3b4)) ? ++c1 : c1;
    
    // Round
    
    c1 = checkHighBit(t1) ? ++c1 : c1;
    
    // Sum the fractional part
    
    uint64_t lo = 0ULL;
    unsigned int c2 = addWithCarry(&lo, a2b4, a4b2) ? 1U : 0U;
    c2 = addWithCarry(&lo, lo, a3b3) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, joinBits(a1b4, a4b3)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, joinBits(a4b1, a3b4)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, loToHiBits(a2b3)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, loToHiBits(a3b2) | c1) ? ++c2: c2;
    
    // Sum the integer part
    
    uint64_t hi = 0ULL;
    unsigned int c3 = addWithCarry(&hi, a2b2, a1b3) ? 1U : 0U;
    c3 = addWithCarry(&hi, hi, a3b1) ? ++c3: c3;
    c3 = addWithCarry(&hi, hi, joinBits(a1b2, a1b4)) ? ++c3: c3;
    c3 = addWithCarry(&hi, hi, joinBits(a2b1, a4b1)) ? ++c3: c3;
    c3 = addWithCarry(&hi, hi, hi32Bits(a2b3) + hi32Bits(a3b2)) ? ++c3: c3;
    c3 = addWithCarry(&hi, hi, c2) ? ++c3: c3;
    
    // Do overflow
    
    t1 = (a1b1 | hi32Bits(a1b2) | hi32Bits(a2b1) | c3) ? FL_Limits<uint64_t>::largest() : 0ULL;
    
    return FL_FP(hi | t1, lo | t1);
}

FL_FP operator / (const FL_FP& a, const FL_FP& b)
{
    // Estimate reciprocal using doubles
    
    const FL_FP rE(1.0 / (double) b);
    
    // Newton-Raphson
    
    // FIX - maybe 2 is enough iterations?
    
    FL_SP r = FL_SP(rE.mInt, rE.mFrac, 0);
    r = qMul(twoMinus(qMul(r, b.mInt, b.mFrac)), rE.mInt, rE.mFrac);
    r = r * twoMinus(qMul(r, b.mInt, b.mFrac));
    //r = r * twoMinus(qMul(r, b.mInt, b.mFrac));
    
    // Multiply reciprocal to get final result and then convert to lower precision
    
    return FL_FP(qMul(r, a.mInt, a.mFrac));
};