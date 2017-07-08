
#include "FrameLib_FixedPoint.h"

// Utility

// Bit Manipulation

uint64_t lo32Bits(uint64_t a)                        { return a & 0xFFFFFFFF; }
uint64_t hi32Bits(uint64_t a)                        { return a >> 0x20; }
uint64_t joinBits(uint64_t hi, uint64_t lo)          { return (hi << 0x20) | hi32Bits(lo); }
uint64_t highBits(uint64_t a)                        { return (a << 0x20); }
bool checkHighBit(uint64_t a)                        { return (a & 0x8000000000000000ULL) ? true : false; }

// Single 64 bit sum/add/subtract with carry bit

void sumWithCarry(FUInt64& sum, FUInt64& carry, const FUInt64& add)
{
    sum += add;
    
    carry = (sum < add) ? ++carry: carry;
}

bool addWithCarry(FUInt64 *result, const FUInt64& a, const FUInt64& b)
{
    *result = a + b;
    return (*result < b);
}

bool subWithCarry(FUInt64 *result, const FUInt64& a, const FUInt64& b)
{
    *result = a - b;
    return (*result > a);
}

// ************************************************************************************** //

FL_SP qMul(const FL_SP& a, const FUInt64 &intVal, const FUInt64 &fracVal)
{
    // Split both into 6/4 x 32 bits stored within 64 bit integers
    
    const FUInt64 a1 = hi32Bits(a.mInt);
    const FUInt64 a2 = lo32Bits(a.mInt);
    const FUInt64 a3 = hi32Bits(a.mFracHi);
    const FUInt64 a4 = lo32Bits(a.mFracHi);
    const FUInt64 a5 = hi32Bits(a.mFracLo);
    const FUInt64 a6 = lo32Bits(a.mFracLo);
    
    const FUInt64 b1 = hi32Bits(intVal);
    const FUInt64 b2 = lo32Bits(intVal);
    const FUInt64 b3 = hi32Bits(fracVal);
    const FUInt64 b4 = lo32Bits(fracVal);
    
    const FUInt64 a1b1 = a1 * b1;
    const FUInt64 a1b2 = a1 * b2;
    const FUInt64 a1b3 = a1 * b3;
    const FUInt64 a1b4 = a1 * b4;
    
    const FUInt64 a2b1 = a2 * b1;
    const FUInt64 a2b2 = a2 * b2;
    const FUInt64 a2b3 = a2 * b3;
    const FUInt64 a2b4 = a2 * b4;
    
    const FUInt64 a3b1 = a3 * b1;
    const FUInt64 a3b2 = a3 * b2;
    const FUInt64 a3b3 = a3 * b3;
    const FUInt64 a3b4 = a3 * b4;
    
    const FUInt64 a4b1 = a4 * b1;
    const FUInt64 a4b2 = a4 * b2;
    const FUInt64 a4b3 = a4 * b3;
    const FUInt64 a4b4 = a4 * b4;
    
    const FUInt64 a5b1 = a5 * b1;
    const FUInt64 a5b2 = a5 * b2;
    const FUInt64 a5b3 = a5 * b3;
    const FUInt64 a5b4 = a5 * b4;
    
    const FUInt64 a6b1 = a6 * b1;
    const FUInt64 a6b2 = a6 * b2;
    const FUInt64 a6b3 = a6 * b3;
    const FUInt64 a6b4 = a6 * b4;
    
    // Lowest carry bits
    
    FUInt64 c1(0U);
    FUInt64 t1 = a6b4;
    sumWithCarry(t1, c1, highBits(a6b3));
    sumWithCarry(t1, c1, highBits(a5b4));
    
    // Round
    
    c1 = checkHighBit(t1) ? ++c1 : c1;
    
    // Sum the lo fractional part
    
    FUInt64 c2(0U);
    FUInt64 lo = a6b2;
    sumWithCarry(lo, c2, a5b3);
    sumWithCarry(lo, c2, a4b4);
    sumWithCarry(lo, c2, joinBits(a6b1, a6b3));
    sumWithCarry(lo, c2, joinBits(a5b2, a5b4));
    sumWithCarry(lo, c2, highBits(a4b3));
    sumWithCarry(lo, c2, highBits(a3b4) | c1);
    
    // Sum the hi fractional part
    
    FUInt64 c3(0U);
    FUInt64 md = a2b4;
    sumWithCarry(md, c3, a4b2);
    sumWithCarry(md, c3, a3b3);
    sumWithCarry(md, c3, a5b1);
    sumWithCarry(md, c3, joinBits(a1b4, a4b3));
    sumWithCarry(md, c3, joinBits(a4b1, a3b4));
    sumWithCarry(md, c3, joinBits(a2b3, a6b1));
    sumWithCarry(md, c3, joinBits(a3b2, a5b2));
    sumWithCarry(md, c3, c2);
    
    // Sum the integer part
    
    FUInt64 c4(0U);
    FUInt64 hi = a2b2;
    sumWithCarry(hi, c4, a1b3);
    sumWithCarry(hi, c4, a3b1);
    sumWithCarry(hi, c4, joinBits(a1b2, a1b4));
    sumWithCarry(hi, c4, joinBits(a2b1, a4b1));
    sumWithCarry(hi, c4, hi32Bits(a2b3) + hi32Bits(a3b2));
    sumWithCarry(hi, c4, c3);
    
    // Do overflow
    
    t1 = (a1b1 | hi32Bits(a1b2) | hi32Bits(a2b1) | c4) ? FL_Limits<FUInt64>::largest() : FUInt64(0U);
    
    return FL_SP(hi | t1, md | t1, lo | t1);
}

FL_SP operator * (const FL_SP& a, const FL_SP& b)
{
    // Split both into 6 x 32 bits stored within 64 bit integers
    
    const FUInt64 a1 = hi32Bits(a.mInt);
    const FUInt64 a2 = lo32Bits(a.mInt);
    const FUInt64 a3 = hi32Bits(a.mFracHi);
    const FUInt64 a4 = lo32Bits(a.mFracHi);
    const FUInt64 a5 = hi32Bits(a.mFracLo);
    const FUInt64 a6 = lo32Bits(a.mFracLo);
    
    const FUInt64 b1 = hi32Bits(b.mInt);
    const FUInt64 b2 = lo32Bits(b.mInt);
    const FUInt64 b3 = hi32Bits(b.mFracHi);
    const FUInt64 b4 = lo32Bits(b.mFracHi);
    const FUInt64 b5 = hi32Bits(b.mFracLo);
    const FUInt64 b6 = lo32Bits(b.mFracLo);
    
    const FUInt64 a1b1 = a1 * b1;
    const FUInt64 a1b2 = a1 * b2;
    const FUInt64 a1b3 = a1 * b3;
    const FUInt64 a1b4 = a1 * b4;
    const FUInt64 a1b5 = a1 * b5;
    const FUInt64 a1b6 = a1 * b6;
    
    const FUInt64 a2b1 = a2 * b1;
    const FUInt64 a2b2 = a2 * b2;
    const FUInt64 a2b3 = a2 * b3;
    const FUInt64 a2b4 = a2 * b4;
    const FUInt64 a2b5 = a2 * b5;
    const FUInt64 a2b6 = a2 * b6;
    
    const FUInt64 a3b1 = a3 * b1;
    const FUInt64 a3b2 = a3 * b2;
    const FUInt64 a3b3 = a3 * b3;
    const FUInt64 a3b4 = a3 * b4;
    const FUInt64 a3b5 = a3 * b5;
    const FUInt64 a3b6 = a3 * b6;
    
    const FUInt64 a4b1 = a4 * b1;
    const FUInt64 a4b2 = a4 * b2;
    const FUInt64 a4b3 = a4 * b3;
    const FUInt64 a4b4 = a4 * b4;
    const FUInt64 a4b5 = a4 * b5;
    const FUInt64 a4b6 = a4 * b6;
    
    const FUInt64 a5b1 = a5 * b1;
    const FUInt64 a5b2 = a5 * b2;
    const FUInt64 a5b3 = a5 * b3;
    const FUInt64 a5b4 = a5 * b4;
    const FUInt64 a5b5 = a5 * b5;
    const FUInt64 a5b6 = a5 * b6;
    
    const FUInt64 a6b1 = a6 * b1;
    const FUInt64 a6b2 = a6 * b2;
    const FUInt64 a6b3 = a6 * b3;
    const FUInt64 a6b4 = a6 * b4;
    const FUInt64 a6b5 = a6 * b5;
    const FUInt64 a6b6 = a6 * b6;
    
    // Sub carry bits
    
    FUInt64 c0(0U);
    FUInt64 t1 = a6b6;
    sumWithCarry(t1, c0, highBits(a5b6));
    sumWithCarry(t1, c0, highBits(a6b5));
    
    // Lowest carry bits
    
    FUInt64 c1(0U);
    t1 = a4b6;
    sumWithCarry(t1, c1, a6b4);
    sumWithCarry(t1, c1, a5b5);
    sumWithCarry(t1, c1, joinBits(a3b6, a5b6));
    sumWithCarry(t1, c1, joinBits(a6b3, a6b5));
    sumWithCarry(t1, c1, highBits(a4b5));
    sumWithCarry(t1, c1, highBits(a5b4) | c0);
    
    // Round
    
    c1 = checkHighBit(t1) ? ++c1 : c1;
    
    // Sum the lo fractional part
    
    FUInt64 c2(0U);
    FUInt64 lo = a2b6;
    sumWithCarry(lo, c2, a6b2);
    sumWithCarry(lo, c2, a3b5);
    sumWithCarry(lo, c2, a5b3);
    sumWithCarry(lo, c2, a4b4);
    sumWithCarry(lo, c2, joinBits(a1b6, a3b6));
    sumWithCarry(lo, c2, joinBits(a6b1, a6b3));
    sumWithCarry(lo, c2, joinBits(a2b5, a4b5));
    sumWithCarry(lo, c2, joinBits(a5b2, a5b4));
    sumWithCarry(lo, c2, highBits(a4b3));
    sumWithCarry(lo, c2, highBits(a3b4) | c1);
    
    // Sum the hi fractional part
    
    FUInt64 c3(0U);
    FUInt64 md = a2b4;
    sumWithCarry(md, c3, a4b2);
    sumWithCarry(md, c3, a3b3);
    sumWithCarry(md, c3, a1b5);
    sumWithCarry(md, c3, a5b1);
    sumWithCarry(md, c3, joinBits(a1b4, a4b3));
    sumWithCarry(md, c3, joinBits(a4b1, a3b4));
    sumWithCarry(md, c3, joinBits(a2b3, a1b6));
    sumWithCarry(md, c3, joinBits(a3b2, a6b1));
    sumWithCarry(md, c3, hi32Bits(a2b5) + hi32Bits(a5b2));
    sumWithCarry(md, c3, c2);
    
    // Sum the integer part
    
    FUInt64 c4(0U);
    FUInt64 hi = a2b2;
    sumWithCarry(hi, c4, a1b3);
    sumWithCarry(hi, c4, a3b1);
    sumWithCarry(hi, c4, joinBits(a1b2, a1b4));
    sumWithCarry(hi, c4, joinBits(a2b1, a4b1));
    sumWithCarry(hi, c4, hi32Bits(a2b3) + hi32Bits(a3b2));
    sumWithCarry(hi, c4, c3);
    
    // Do overflow
    
    t1 = (a1b1 | hi32Bits(a1b2) | hi32Bits(a2b1) | c4) ? FL_Limits<FUInt64>::largest() : FUInt64(0U);
    
    return FL_SP(hi | t1, md | t1, lo | t1);
}

FL_SP operator - (const FL_SP& a, const FL_SP& b)
{
    FUInt64 hi, md, lo;
    
    hi = a.mInt - b.mInt;
    hi = subWithCarry(&md, a.mFracHi, b.mFracHi) ? --hi : hi;
    
    // FIX - needs more checking!!
    // Must be able to double carry!
    
    if (subWithCarry(&lo, a.mFracLo, b.mFracLo))
    {
        hi = md ? hi : --hi;
        --md;
    }
    
    return FL_SP(hi, md, lo);
}

FL_SP twoMinus(const FL_SP& b)
{
    // FIX - needs more checking!!
    
    FUInt64 hi = FUInt64(2U) - b.mInt;
    FUInt64 md = FUInt64(0U) - b.mFracHi;
    FUInt64 lo = FUInt64(0U) - b.mFracLo;
    
    // Double carry!
    
    if ((b.mFracHi | b.mFracLo))
        --hi;
    
    if (b.mFracLo)
        --md;
    
    return FL_SP(hi, md, lo);
}


// ************************************************************************************** //

// FL_FP

// This class provides a very high precision fixed point format for dealing with time

FL_FP::FL_FP(double val)
{
    double absVal = fabs(val);
    
    if (18446744073709551616.0 <= absVal)
        mInt = mFrac = FL_Limits<FUInt64>::largest();
    else
    {
        mInt = absVal;
        mFrac = ((absVal - floor(absVal)) * 18446744073709551616.0);
    }
}

FL_FP operator + (const FL_FP& a, const FL_FP& b)
{
    FUInt64 hi, lo;
    
    hi = a.mInt + b.mInt;
    hi = addWithCarry(&lo, a.mFrac, b.mFrac) ? ++hi : hi;
    
    return FL_FP(hi, lo);
}

FL_FP operator - (const FL_FP& a, const FL_FP& b)
{
    FUInt64 hi, lo;
    
    hi = a.mInt - b.mInt;
    hi = subWithCarry(&lo, a.mFrac, b.mFrac) ? --hi : hi;
    
    return FL_FP(hi, lo);
}

FL_FP operator * (const FL_FP& a, const FL_FP& b)
{    
    FUInt64 c1, c2, c3;
    FUInt64 t1, hi, lo;
    
    // Split both into 4 x 32 bits stored within 64 bit integers
    
    const FUInt64 a1 = hi32Bits(a.mInt);
    const FUInt64 a2 = lo32Bits(a.mInt);
    const FUInt64 a3 = hi32Bits(a.mFrac);
    const FUInt64 a4 = lo32Bits(a.mFrac);
    
    const FUInt64 b1 = hi32Bits(b.mInt);
    const FUInt64 b2 = lo32Bits(b.mInt);
    const FUInt64 b3 = hi32Bits(b.mFrac);
    const FUInt64 b4 = lo32Bits(b.mFrac);
    
    const FUInt64 a1b1 = a1 * b1;
    const FUInt64 a1b2 = a1 * b2;
    const FUInt64 a1b3 = a1 * b3;
    const FUInt64 a1b4 = a1 * b4;
    
    const FUInt64 a2b1 = a2 * b1;
    const FUInt64 a2b2 = a2 * b2;
    const FUInt64 a2b3 = a2 * b3;
    const FUInt64 a2b4 = a2 * b4;
    
    const FUInt64 a3b1 = a3 * b1;
    const FUInt64 a3b2 = a3 * b2;
    const FUInt64 a3b3 = a3 * b3;
    const FUInt64 a3b4 = a3 * b4;
    
    const FUInt64 a4b1 = a4 * b1;
    const FUInt64 a4b2 = a4 * b2;
    const FUInt64 a4b3 = a4 * b3;
    const FUInt64 a4b4 = a4 * b4;
    
    // Lowest carry bits
    
    c1 = addWithCarry(&t1, a4b4, highBits(a4b3)) ? FUInt64(1U) : FUInt64(0U);
    c1 = addWithCarry(&t1, t1, highBits(a3b4)) ? ++c1 : c1;
    c1 = checkHighBit(t1) ? ++c1 : c1;
    
    // Sum the fractional part
    
    c2 = addWithCarry(&lo, a2b4, a4b2) ? FUInt64(1U) : FUInt64(0U);
    c2 = addWithCarry(&lo, lo, a3b3) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, joinBits(a1b4, a4b3)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, joinBits(a4b1, a3b4)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, highBits(a2b3)) ? ++c2: c2;
    c2 = addWithCarry(&lo, lo, highBits(a3b2) | c1) ? ++c2: c2;
    
    // Sum the integer part
    
    c3 = addWithCarry(&hi, a2b2, a1b3) ? FUInt64(1U) : FUInt64(0U);
    c3 = addWithCarry(&hi, hi, a3b1) ? ++c3: c3;
    c3 = addWithCarry(&hi, hi, joinBits(a1b2, a1b4)) ? ++c3: c3;
    c3 = addWithCarry(&hi, hi, joinBits(a2b1, a4b1)) ? ++c3: c3;
    c3 = addWithCarry(&hi, hi, hi32Bits(a2b3) + hi32Bits(a3b2)) ? ++c3: c3;
    c3 = addWithCarry(&hi, hi, c2) ? ++c3: c3;
    
    // Do overflow
    
    t1 = (a1b1 | hi32Bits(a1b2) | hi32Bits(a2b1) | c3) ? FL_Limits<FUInt64>::largest() : FUInt64(0U);
    
    return FL_FP(hi | t1, lo | t1);
}

FL_FP operator / (const FL_FP& a, const FL_FP& b)
{
    // Estimate using doubles
    
    const FL_FP recipEst(1.0 / (double) b);
    
    // Newton-Raphson
    
    // FIX - maybe 2 is enough iterations?
    FL_SP recip = FL_SP(recipEst.mInt, recipEst.mFrac, FUInt64(0U));
    recip = qMul(twoMinus(qMul(recip, b.mInt, b.mFrac)), recipEst.mInt, recipEst.mFrac);
    recip *= twoMinus(qMul(recip, b.mInt, b.mFrac));
    //recip *= twoMinus(qMul(recip, b.mInt, b.mFrac));
    
    // Multiply reciprocal to get final result and then round
    
    const FL_SP result = qMul(recip, a.mInt, a.mFrac);
    
    FL_FP resultLP = FL_FP(result.intVal(), result.fracHiVal());
    resultLP = checkHighBit(result.fracLoVal()) ? resultLP + FL_FP(0U, 1U) : resultLP;
    
    return resultLP;
};