
#ifndef FRAMELIB_FIXEDPOINT_H
#define FRAMELIB_FIXEDPOINT_H

#include "FrameLib_Types.h"
#include <cmath>
#include <limits>

// This needs to be altered to cope with platforms other than windows/mac and compilers other than visual studio and GCC

#ifdef __APPLE__
#if __LP64__
#define FL_64BIT
#endif
#else
#ifdef _WIN64
#define FL_64BIT
#endif
#endif

// ************************************************************************************** //

// Numeric Limits

template <class T> struct FL_Limits
{
    static T largest()
    {
        if (std::numeric_limits<T>::has_infinity)
            return std::numeric_limits<T>::infinity();
        else
            return std::numeric_limits<T>::max();
    }
};

// ************************************************************************************** //

class FL_SoftUInt64
{

public:
    
    FL_SoftUInt64()
    {
        mHi = 0;
        mLo = 0;
    }
    
    FL_SoftUInt64(uint32_t hi, uint32_t lo)
    {
        mHi = hi;
        mLo = lo;
    }
 
    friend bool operator < (const FL_SoftUInt64& lhs, const FL_SoftUInt64& rhs)
    {
        return ((lhs.mHi < rhs.mHi) || (lhs.mHi == rhs.mHi && lhs.mLo < rhs.mLo));
    }
    
    friend bool operator > (const FL_SoftUInt64& lhs, const FL_SoftUInt64& rhs)
    {
        return ((lhs.mHi > rhs.mHi) || (lhs.mHi == rhs.mHi && lhs.mLo > rhs.mLo));
    }

    friend bool operator == (const FL_SoftUInt64& lhs, const FL_SoftUInt64& rhs)
    {
        return (lhs.mHi == rhs.mHi && lhs.mLo == rhs.mLo);
    }
    
    friend bool operator ! ( const FL_SoftUInt64& rhs)
    {
        return !rhs.mHi && !rhs.mLo;
    }
    
    friend FL_SoftUInt64 operator + (const FL_SoftUInt64& lhs, const FL_SoftUInt64& rhs)
    {
        uint32_t hi, lo;
        
        hi = lhs.mHi + rhs.mHi;
        lo = lhs.mLo + rhs.mHi;
        
        hi = (lo < lhs.mLo) ? ++hi : hi;
        
        return FL_SoftUInt64(hi, lo);
    }
    
    friend FL_SoftUInt64 operator - (const FL_SoftUInt64& lhs, const FL_SoftUInt64& rhs)
    {
        uint32_t hi, lo;
        
        hi = lhs.mHi - rhs.mHi;
        lo = lhs.mLo - rhs.mHi;
        
        hi = (lo > lhs.mLo) ? --hi : hi;
        
        return FL_SoftUInt64(hi, lo);
    }
    
    operator double() const
    {
        return ((double) mHi * 4294967296.0) + ((double) mLo);
    }
    
    FL_SoftUInt64& operator -- ()
    {
        mHi = (++mLo == 0) ? ++mHi : mHi;
        return *this;
    }

    FL_SoftUInt64& operator ++ (int)
    {
        FL_SoftUInt64& result = *this;
        operator++();
        return result;
    }
    
    FL_SoftUInt64& operator ++ ()
    {
        mHi = (mLo-- == 0) ? --mHi : mHi;
        return *this;
    }
    
    FL_SoftUInt64& operator -- (int)
    {
        FL_SoftUInt64& result = *this;
        operator--();
        return result;
    }
    
    // The double is aassumed to be unsigned here
    
    FL_SoftUInt64& operator = (const double& rhs)
    {
        mLo = (uint32_t) (rhs / 4294967296.0);
        mHi = (uint32_t) (rhs - (mLo * 4294967296));
        
        return *this;
    }
    
private:
    
    uint32_t mHi;
    uint32_t mLo;
};

// ************************************************************************************** //

template<> struct FL_Limits <FL_SoftUInt64>
{
    static FL_SoftUInt64 largest()
    {
        return FL_SoftUInt64(std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max());
    }
};


// ************************************************************************************** //

//#ifdef FL_64BIT
typedef uint64_t FL_Internal_UInt64;
//#else
//typedef FL_SoftUInt64 FL_Internal_UInt64;
//#endif

// ************************************************************************************** //

struct SuperPrecision
{
    SuperPrecision()
    {
        mInt = FL_Internal_UInt64();
        mFracHi = FL_Internal_UInt64();
        mFracLo = FL_Internal_UInt64();
    }
    
    SuperPrecision(FL_Internal_UInt64 intVal, FL_Internal_UInt64 fracValHi, FL_Internal_UInt64 fracValLo)
    {
        mInt = intVal;
        mFracHi = fracValHi;
        mFracLo = fracValLo;
    }
    
    FL_Internal_UInt64 intPart()
    {
        return mInt;
    }
    
    FL_Internal_UInt64 fracHiPart()
    {
        return mFracHi;
    }
   
    FL_Internal_UInt64 fracLoPart()
    {
        return mFracLo;
    }
    
    static uint64_t lowBits(uint64_t a)
    {
        return a & 0xFFFFFFFF;
    }
    
    static uint64_t highBits(uint64_t a)
    {
        return a >> 0x20;
    }
    
    static uint64_t combineBits(uint64_t hi, uint64_t lo)
    {
        return (hi << 0x20) | highBits(lo);
    }
    
    friend SuperPrecision qMul (const SuperPrecision& lhs, const uint64_t &intPart, const uint64_t &fracPart)
    {
        // Split both into 4 x 32 bits stored within 64 bit integers
        
        uint64_t a1 = highBits(lhs.mInt);
        uint64_t a2 = lowBits(lhs.mInt);
        uint64_t a3 = highBits(lhs.mFracHi);
        uint64_t a4 = lowBits(lhs.mFracHi);
        uint64_t a5 = highBits(lhs.mFracLo);
        uint64_t a6 = lowBits(lhs.mFracLo);
        
        uint64_t b1 = highBits(intPart);
        uint64_t b2 = lowBits(intPart);
        uint64_t b3 = highBits(fracPart);
        uint64_t b4 = lowBits(fracPart);
        
        uint64_t a1b1 = a1 * b1;
        uint64_t a1b2 = a1 * b2;
        uint64_t a1b3 = a1 * b3;
        uint64_t a1b4 = a1 * b4;
        
        uint64_t a2b1 = a2 * b1;
        uint64_t a2b2 = a2 * b2;
        uint64_t a2b3 = a2 * b3;
        uint64_t a2b4 = a2 * b4;
        
        uint64_t a3b1 = a3 * b1;
        uint64_t a3b2 = a3 * b2;
        uint64_t a3b3 = a3 * b3;
        uint64_t a3b4 = a3 * b4;
        
        uint64_t a4b1 = a4 * b1;
        uint64_t a4b2 = a4 * b2;
        uint64_t a4b3 = a4 * b3;
        uint64_t a4b4 = a4 * b4;
        
        uint64_t a5b1 = a5 * b1;
        uint64_t a5b2 = a5 * b2;
        uint64_t a5b3 = a5 * b3;
        uint64_t a5b4 = a5 * b4;
        
        uint64_t a6b1 = a6 * b1;
        uint64_t a6b2 = a6 * b2;
        uint64_t a6b3 = a6 * b3;
        uint64_t a6b4 = a6 * b4;
        
        // Lowest carry bits
        
        uint64_t c1 = 0;
        uint64_t t1 = a6b4;
        sumWithCarry(t1, c1, (a6b3 << 0x20));
        sumWithCarry(t1, c1, (a5b4 << 0x20));
        
        // Round
        
        c1 = (t1 & 0x8000000000000000ULL) ? ++c1 : c1;
        
        // Sum the lo fractional part
        
        uint64_t c2 = 0;
        uint64_t lo = a6b2;
        sumWithCarry(lo, c2, a5b3);
        sumWithCarry(lo, c2, a4b4);
        sumWithCarry(lo, c2, combineBits(a6b1, a6b3));
        sumWithCarry(lo, c2, combineBits(a5b2, a5b4));
        sumWithCarry(lo, c2, combineBits(a4b3, 0));
        sumWithCarry(lo, c2, (a3b4 << 0x20) | c1);
        
        // Sum the hi fractional part
        
        uint64_t c3 = 0;
        uint64_t md = a2b4;
        sumWithCarry(md, c3, a4b2);
        sumWithCarry(md, c3, a3b3);
        sumWithCarry(md, c3, a5b1);
        sumWithCarry(md, c3, combineBits(a1b4, a4b3));
        sumWithCarry(md, c3, combineBits(a4b1, a3b4));
        sumWithCarry(md, c3, combineBits(a2b3, a6b1));
        sumWithCarry(md, c3, combineBits(a3b2, a5b2));
        sumWithCarry(md, c3, c2);
        
        // Sum the integer part
        
        uint64_t c4 = 0;
        uint64_t hi = a2b2;
        sumWithCarry(hi, c4, a1b3);
        sumWithCarry(hi, c4, a3b1);
        sumWithCarry(hi, c4, combineBits(a1b2, a1b4));
        sumWithCarry(hi, c4, combineBits(a2b1, a4b1));
        sumWithCarry(hi, c4, highBits(a2b3) + highBits(a3b2));
        sumWithCarry(hi, c4, c3);
        
        // Do overflow
        
        t1 = (a1b1 | highBits(a1b2) | highBits(a2b1) | c4) ? 0xFFFFFFFFFFFFFFFFULL : 0ULL;
        
        return SuperPrecision(hi | t1, md | t1, lo | t1);
    }
    
    friend SuperPrecision operator * (const SuperPrecision& lhs, const SuperPrecision& rhs)
    {
        // Split both into 4 x 32 bits stored within 64 bit integers
        
        uint64_t a1 = highBits(lhs.mInt);
        uint64_t a2 = lowBits(lhs.mInt);
        uint64_t a3 = highBits(lhs.mFracHi);
        uint64_t a4 = lowBits(lhs.mFracHi);
        uint64_t a5 = highBits(lhs.mFracLo);
        uint64_t a6 = lowBits(lhs.mFracLo);
        
        uint64_t b1 = highBits(rhs.mInt);
        uint64_t b2 = lowBits(rhs.mInt);
        uint64_t b3 = highBits(rhs.mFracHi);
        uint64_t b4 = lowBits(rhs.mFracHi);
        uint64_t b5 = highBits(rhs.mFracLo);
        uint64_t b6 = lowBits(rhs.mFracLo);
        
        uint64_t a1b1 = a1 * b1;
        uint64_t a1b2 = a1 * b2;
        uint64_t a1b3 = a1 * b3;
        uint64_t a1b4 = a1 * b4;
        uint64_t a1b5 = a1 * b5;
        uint64_t a1b6 = a1 * b6;
        
        uint64_t a2b1 = a2 * b1;
        uint64_t a2b2 = a2 * b2;
        uint64_t a2b3 = a2 * b3;
        uint64_t a2b4 = a2 * b4;
        uint64_t a2b5 = a2 * b5;
        uint64_t a2b6 = a2 * b6;
        
        uint64_t a3b1 = a3 * b1;
        uint64_t a3b2 = a3 * b2;
        uint64_t a3b3 = a3 * b3;
        uint64_t a3b4 = a3 * b4;
        uint64_t a3b5 = a3 * b5;
        uint64_t a3b6 = a3 * b6;
        
        uint64_t a4b1 = a4 * b1;
        uint64_t a4b2 = a4 * b2;
        uint64_t a4b3 = a4 * b3;
        uint64_t a4b4 = a4 * b4;
        uint64_t a4b5 = a4 * b5;
        uint64_t a4b6 = a4 * b6;
        
        uint64_t a5b1 = a5 * b1;
        uint64_t a5b2 = a5 * b2;
        uint64_t a5b3 = a5 * b3;
        uint64_t a5b4 = a5 * b4;
        uint64_t a5b5 = a5 * b5;
        uint64_t a5b6 = a5 * b6;
        
        uint64_t a6b1 = a6 * b1;
        uint64_t a6b2 = a6 * b2;
        uint64_t a6b3 = a6 * b3;
        uint64_t a6b4 = a6 * b4;
        uint64_t a6b5 = a6 * b5;
        uint64_t a6b6 = a6 * b6;

        // Sub carry bits

        uint64_t c0 = 0;
        uint64_t t1 = a6b6;
        sumWithCarry(t1, c0, (a5b6 << 0x20));
        sumWithCarry(t1, c0, (a6b5 << 0x20));

        // Lowest carry bits
        
        uint64_t c1 = 0;
        t1 = a4b6;
        sumWithCarry(t1, c1, a6b4);
        sumWithCarry(t1, c1, a5b5);
        sumWithCarry(t1, c1, combineBits(a3b6, a5b6));
        sumWithCarry(t1, c1, combineBits(a6b3, a6b5));
        sumWithCarry(t1, c1, combineBits(a4b5, 0));
        sumWithCarry(t1, c1, (a5b4 << 0x20) | c0);
        
        // Round
        
        c1 = (t1 & 0x8000000000000000ULL) ? ++c1 : c1;
        
        // Sum the lo fractional part
        
        uint64_t c2 = 0;
        uint64_t lo = a2b6;
        sumWithCarry(lo, c2, a6b2);
        sumWithCarry(lo, c2, a3b5);
        sumWithCarry(lo, c2, a5b3);
        sumWithCarry(lo, c2, a4b4);
        sumWithCarry(lo, c2, combineBits(a1b6, a3b6));
        sumWithCarry(lo, c2, combineBits(a6b1, a6b3));
        sumWithCarry(lo, c2, combineBits(a2b5, a4b5));
        sumWithCarry(lo, c2, combineBits(a5b2, a5b4));
        sumWithCarry(lo, c2, combineBits(a4b3, 0));
        sumWithCarry(lo, c2, (a3b4 << 0x20) | c1);
        
        // Sum the hi fractional part
        
        uint64_t c3 = 0;
        uint64_t md = a2b4;
        sumWithCarry(md, c3, a4b2);
        sumWithCarry(md, c3, a3b3);
        sumWithCarry(md, c3, a1b5);
        sumWithCarry(md, c3, a5b1);
        sumWithCarry(md, c3, combineBits(a1b4, a4b3));
        sumWithCarry(md, c3, combineBits(a4b1, a3b4));
        sumWithCarry(md, c3, combineBits(a2b3, a1b6));
        sumWithCarry(md, c3, combineBits(a3b2, a6b1));
        sumWithCarry(md, c3, highBits(a2b5) + highBits(a5b2));
        sumWithCarry(md, c3, c2);

        // Sum the integer part
        
        uint64_t c4 = 0;
        uint64_t hi = a2b2;
        sumWithCarry(hi, c4, a1b3);
        sumWithCarry(hi, c4, a3b1);
        sumWithCarry(hi, c4, combineBits(a1b2, a1b4));
        sumWithCarry(hi, c4, combineBits(a2b1, a4b1));
        sumWithCarry(hi, c4, highBits(a2b3) + highBits(a3b2));
        sumWithCarry(hi, c4, c3);
        
        // Do overflow
        
        t1 = (a1b1 | highBits(a1b2) | highBits(a2b1) | c4) ? 0xFFFFFFFFFFFFFFFFULL : 0ULL;
        
        return SuperPrecision(hi | t1, md | t1, lo | t1);
    }

    SuperPrecision& operator *= (const SuperPrecision& rhs)
    {
        *this = *this * rhs;
        return *this;
    }
    
    friend SuperPrecision operator - (const SuperPrecision& lhs, const SuperPrecision& rhs)
    {
        FL_Internal_UInt64 hi, md, lo;
        
        hi = lhs.mInt - rhs.mInt;
        hi = subWithCarry(&md, lhs.mFracHi, rhs.mFracHi) ? --hi : hi;
        
        // FIX - needs more checking!!
        // Must be able to double carry!
        
        if (subWithCarry(&lo, lhs.mFracLo, rhs.mFracLo))
        {
            hi = md ? hi : --hi;
            --md;
        }

        return SuperPrecision(hi, md, lo);
    }
    
    friend SuperPrecision twoMinus(const SuperPrecision& rhs)
    {
        // FIX - needs more checking!!
        
        FL_Internal_UInt64 hi =  2 - rhs.mInt;
        FL_Internal_UInt64 md = -rhs.mFracHi;
        FL_Internal_UInt64 lo = -rhs.mFracLo;
        
        // Double carry!
        
        if ((rhs.mFracHi | rhs.mFracLo))
            --hi;
        
        if (rhs.mFracLo)
            --md;
    
        return SuperPrecision(hi, md, lo);
    }

    friend bool operator > (const SuperPrecision& lhs, const SuperPrecision& rhs)
    {
        return ((lhs.mInt > rhs.mInt) || (lhs.mInt == rhs.mInt && lhs.mFracHi > rhs.mFracHi) || (lhs.mInt == rhs.mInt && lhs.mFracHi == rhs.mFracHi && lhs.mFracLo > rhs.mFracLo));
    }
    
private:
    
    
    
    // Single 64 bit add/subtract with carry bit
    
    static void sumWithCarry(FL_Internal_UInt64& sum, FL_Internal_UInt64& carry, const FL_Internal_UInt64& add)
    {
        sum += add;
        
        carry = (sum < add) ? ++carry: carry;
    }
    
    static inline bool subWithCarry(FL_Internal_UInt64 *result, const FL_Internal_UInt64& a, const FL_Internal_UInt64& b)
    {
        *result = a - b;
        return (*result > a);
    }

    
private:
    
    FL_Internal_UInt64 mInt;
    FL_Internal_UInt64 mFracHi;
    FL_Internal_UInt64 mFracLo;
};

// ************************************************************************************** //

// FrameLib_FixedPoint

// This class provides a very high precision fixed point format for dealing with time

class FrameLib_FixedPoint
{

public:
    
    FrameLib_FixedPoint()
    {
        mInt = FL_Internal_UInt64();
        mFrac = FL_Internal_UInt64();
    }
    
    FrameLib_FixedPoint(FL_Internal_UInt64 intVal, FL_Internal_UInt64 fracVal)
    {
        mInt = intVal;
        mFrac = fracVal;
    }
    
    FrameLib_FixedPoint(double val)
    {
        double absVal = fabs(val);
        
        if (18446744073709551616.0 <= absVal)
            mInt = mFrac = 0xFFFFFFFFFFFFFFFFULL;
        else
        {
            mInt = absVal;
            mFrac = ((absVal - floor(absVal)) * 18446744073709551616.0);
        }
    }
    
    // Int and Fract
    
    FL_Internal_UInt64 intPart()
    {
        return mInt;
    }
    
    FL_Internal_UInt64 fracPart()
    {
        return mFrac;
    }
    
    // Absolute value
    
    friend FrameLib_FixedPoint clipToPositive(const FrameLib_FixedPoint& arg)
    {
        return arg;
    }
    
    // Casting to double
    
    operator double()  const
    {
        return ((double) mInt) + ((double) mFrac / 18446744073709551616.0);
    }
    
    // Comparison operators
    
public:
    
    friend bool operator ! ( const FrameLib_FixedPoint& rhs)
    {
        return !rhs.mInt && !rhs.mFrac;
    }
    
    friend bool operator < (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        return ((lhs.mInt < rhs.mInt) || (lhs.mInt == rhs.mInt && lhs.mFrac < rhs.mFrac));
    }
    
    friend bool operator > (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        return ((lhs.mInt > rhs.mInt) || (lhs.mInt == rhs.mInt && lhs.mFrac > rhs.mFrac));
    }
    
    friend bool operator <= (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        return !(lhs > rhs);
    }
    
    friend bool operator >= (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        return !(lhs < rhs);
    }
    
    friend bool operator == (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        return (lhs.mInt == rhs.mInt && lhs.mFrac == rhs.mFrac); //&& (lhs.mNeg == rhs.mNeg));
    }
    
private:

    // Single 64 bit add/subtract with carry bit

    static bool addWithCarry(FL_Internal_UInt64 *result, const FL_Internal_UInt64& a, const FL_Internal_UInt64& b)
    {
        *result = a + b;
        return (*result < b);
    }
    
    static inline bool subWithCarry(FL_Internal_UInt64 *result, const FL_Internal_UInt64& a, const FL_Internal_UInt64& b)
    {
        *result = a - b;
        return (*result > a);
    }

    // Arithmetic operators (with assignment)

public:
    
    // Addition
    
    friend FrameLib_FixedPoint operator + (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        FL_Internal_UInt64 hi, lo;
        
        hi = lhs.mInt + rhs.mInt;
        hi = addWithCarry(&lo, lhs.mFrac, rhs.mFrac) ? ++hi : hi;
        
        return FrameLib_FixedPoint(hi, lo);
    }

    FrameLib_FixedPoint& operator += (const FrameLib_FixedPoint& rhs)
    {
        *this = *this + rhs;
        return *this;
    }
    
    // Subtraction
    
    friend FrameLib_FixedPoint operator - (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        FL_Internal_UInt64 hi, lo;
      
        hi = lhs.mInt - rhs.mInt;
        hi = subWithCarry(&lo, lhs.mFrac, rhs.mFrac) ? --hi : hi;
        
        return FrameLib_FixedPoint(hi, lo);
    }
    
    FrameLib_FixedPoint& operator -= (const FrameLib_FixedPoint& rhs)
    {
        *this = *this - rhs;
        return *this;
    }
    
    // Multiplication
    
    static uint64_t lowBits(uint64_t a)
    {
        return a & 0xFFFFFFFF;
    }
    
    static uint64_t highBits(uint64_t a)
    {
        return a >> 0x20;
    }
    
    static uint64_t combineBits(uint64_t hi, uint64_t lo)
    {
        return (hi << 0x20) | highBits(lo);
    }
    
    friend FrameLib_FixedPoint operator * (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        // N.B. Overflow behaviour is undefined for multiplication (it is assumed you are using values well within range for the purpose)
        
        uint64_t c1, c2, c3;
        uint64_t t1, hi, lo;
        
        // Split both into 4 x 32 bits stored within 64 bit integers
        
        const uint64_t a1 = highBits(lhs.mInt);
        const uint64_t a2 = lowBits(lhs.mInt);
        const uint64_t a3 = highBits(lhs.mFrac);
        const uint64_t a4 = lowBits(lhs.mFrac);
        
        const uint64_t b1 = highBits(rhs.mInt);
        const uint64_t b2 = lowBits(rhs.mInt);
        const uint64_t b3 = highBits(rhs.mFrac);
        const uint64_t b4 = lowBits(rhs.mFrac);
        
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
        
        c1 = addWithCarry(&t1, a4b4, combineBits(a4b3, 0)) ? 0x1ULL : 0ULL;
        c1 = addWithCarry(&t1, t1, combineBits(a3b4, 0)) ? ++c1 : c1;
        c1 = (t1 & 0x8000000000000000ULL) ? ++c1 : c1;
        
        // Sum the fractional part
        
        c2 = addWithCarry(&lo, a2b4, a4b2) ? 0x1ULL : 0ULL;;
        c2 = addWithCarry(&lo, lo, a3b3) ? ++c2: c2;
        c2 = addWithCarry(&lo, lo, combineBits(a1b4, a4b3)) ? ++c2: c2;
        c2 = addWithCarry(&lo, lo, combineBits(a4b1, a3b4)) ? ++c2: c2;
        c2 = addWithCarry(&lo, lo, combineBits(a2b3, 0)) ? ++c2: c2;
        c2 = addWithCarry(&lo, lo, (a3b2 << 0x20) | c1) ? ++c2: c2;

        // Sum the integer part
        
        c3 = addWithCarry(&hi, a2b2, a1b3) ? 0x1ULL : 0ULL;;
        c3 = addWithCarry(&hi, hi, a3b1) ? ++c3: c3;
        c3 = addWithCarry(&hi, hi, combineBits(a1b2, a1b4)) ? ++c3: c3;
        c3 = addWithCarry(&hi, hi, combineBits(a2b1, a4b1)) ? ++c3: c3;
        c3 = addWithCarry(&hi, hi, highBits(a2b3) + highBits(a3b2)) ? ++c3: c3;
        c3 = addWithCarry(&hi, hi, c2) ? ++c3: c3;
        
        // Do overflow
    
        t1 = (a1b1 | highBits(a1b2) | highBits(a2b1) | c3) ? 0xFFFFFFFFFFFFFFFFULL : 0ULL;
        
        return FrameLib_FixedPoint(hi | t1, lo | t1);
    }
    
    FrameLib_FixedPoint& operator *= (const FrameLib_FixedPoint& rhs)
    {
        *this = *this * rhs;
        return *this;
    }
    
    // Division
    
    friend FrameLib_FixedPoint operator / (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        double recipD = 1.0 / (double) rhs;
        FrameLib_FixedPoint recipEst = recipD;
        SuperPrecision recip = SuperPrecision(recipEst.mInt, recipEst.mFrac, 0);
        
        // Newton-Raphson
        
        // FIX - maybe 2 is enough iterations?
        
        recip = qMul(twoMinus(qMul(recip, rhs.mInt, rhs.mFrac)), recipEst.mInt, recipEst.mFrac);
        recip *= twoMinus(qMul(recip, rhs.mInt, rhs.mFrac));
        //recip *= twoMinus(qMul(recip, rhs.mInt, rhs.mFrac));

        // Multiply reciprocal to get final result and then round
        
        SuperPrecision result = qMul(recip, lhs.mInt, lhs.mFrac);

        FrameLib_FixedPoint resultLP = FrameLib_FixedPoint(result.intPart(), result.fracHiPart());
        resultLP = (result.fracLoPart() & 0x8000000000000000ULL) ? resultLP + FrameLib_FixedPoint(0, 1) : resultLP;

        return resultLP;
    }

    FrameLib_FixedPoint& operator /= (const FrameLib_FixedPoint& rhs)
    {
        *this = *this / rhs;
        return *this;
    }
    
    // Increment and decrement operatiors
    
    FrameLib_FixedPoint& operator ++ ()
    {
        if (++mFrac == FL_Internal_UInt64())
            ++mInt;
        
        return *this;
    }
   
    FrameLib_FixedPoint& operator ++ (int)
    {
        FrameLib_FixedPoint& result = *this;
        operator++();
        return result;
    }
    
    FrameLib_FixedPoint& operator -- ()
    {
        if (mFrac-- == FL_Internal_UInt64())
            --mInt;
        
        return *this;
    }
    
    FrameLib_FixedPoint& operator -- (int)
    {
        FrameLib_FixedPoint& result = *this;
        operator--();
        return result;
    }
    
    // Support for operators using doubles
    
    FrameLib_FixedPoint& operator = (const double& rhs)
    {
        *this = FrameLib_FixedPoint(rhs);
        return *this;
    }
    
    friend bool operator < (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs < FrameLib_FixedPoint(rhs);
    }
    
    friend bool operator < (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) < rhs;
    }
    
    friend bool operator > (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs > FrameLib_FixedPoint(rhs);
    }
    
    friend bool operator > (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) > rhs;
    }
    
    friend bool operator == (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs == FrameLib_FixedPoint(rhs);
    }
    
    friend bool operator == (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) == rhs;
    }
    
    friend bool operator <= (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs <= FrameLib_FixedPoint(rhs);
    }
    
    friend bool operator <= (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) <= rhs;
    }
    
    friend bool operator >= (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs >= FrameLib_FixedPoint(rhs);
    }
    
    friend bool operator >= (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) >= rhs;
    }
    
    friend FrameLib_FixedPoint operator + (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs + FrameLib_FixedPoint(rhs);
    }
    
    friend FrameLib_FixedPoint operator + (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) + rhs;
    }
    
    FrameLib_FixedPoint& operator += (const double& rhs)
    {
        return operator +=(FrameLib_FixedPoint(rhs));
    }
    
    friend FrameLib_FixedPoint operator - (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs - FrameLib_FixedPoint(rhs);
    }
    
    friend FrameLib_FixedPoint operator - (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) - rhs;
    }
    
    FrameLib_FixedPoint& operator -= (const double& rhs)
    {
        return operator -=(FrameLib_FixedPoint(rhs));
    }
    
    friend FrameLib_FixedPoint operator * (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs * FrameLib_FixedPoint(rhs);
    }
    
    friend FrameLib_FixedPoint operator * (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) * rhs;
    }
    
    FrameLib_FixedPoint& operator *= (const double& rhs)
    {
        return operator *=(FrameLib_FixedPoint(rhs));
    }
    
    friend FrameLib_FixedPoint operator / (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs / FrameLib_FixedPoint(rhs);
    }
    
    friend FrameLib_FixedPoint operator / (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) / rhs;
    }
    
    FrameLib_FixedPoint& operator /= (const double& rhs)
    {
        return operator /=(FrameLib_FixedPoint(rhs));
    }
    
private:
    
    FL_Internal_UInt64 mInt;
    FL_Internal_UInt64 mFrac;
};

// ************************************************************************************** //

inline double clipToPositive (double &arg)
{
    return arg < 0 ? 0 : arg;
}

// ************************************************************************************** //

template<> struct FL_Limits <FrameLib_FixedPoint>
{
    static FrameLib_FixedPoint largest()
    {
        return FrameLib_FixedPoint(FL_Limits<FL_Internal_UInt64>::largest(), FL_Limits<FL_Internal_UInt64>::largest());
    }
};

#endif
