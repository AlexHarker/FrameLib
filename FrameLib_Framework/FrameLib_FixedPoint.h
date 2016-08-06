
#ifndef FRAMELIB_FIXEDPOINT_H
#define FRAMELIB_FIXEDPOINT_H

#include "FrameLib_Types.h"
#include <cmath>
#include <limits>

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
    
    FL_SoftUInt64(FL_UInt32 hi, FL_UInt32 lo)
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
        FL_UInt32 hi, lo;
        
        hi = lhs.mHi + rhs.mHi;
        lo = lhs.mLo + rhs.mHi;
        
        hi = (lo < lhs.mLo) ? ++hi : hi;
        
        return FL_SoftUInt64(hi, lo);
    }
    
    friend FL_SoftUInt64 operator - (const FL_SoftUInt64& lhs, const FL_SoftUInt64& rhs)
    {
        FL_UInt32 hi, lo;
        
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
        mLo = (FL_UInt32) (rhs / 4294967296.0);
        mHi = (FL_UInt32) (rhs - (mLo * 4294967296));
        
        return *this;
    }
    
private:
    
    FL_UInt32 mHi;
    FL_UInt32 mLo;
};

// ************************************************************************************** //

template<> struct FL_Limits <FL_SoftUInt64>
{
    static FL_SoftUInt64 largest()
    {
        return FL_SoftUInt64(std::numeric_limits<FL_UInt32>::max(), std::numeric_limits<FL_UInt32>::max());
    }
};


// ************************************************************************************** //

//#ifdef FL_64BIT
typedef FL_UInt64 FL_Internal_UInt64;
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
    
    static FL_UInt64 lowBits(FL_UInt64 a)
    {
        return a & 0xFFFFFFFF;
    }
    
    static FL_UInt64 highBits(FL_UInt64 a)
    {
        return a >> 0x20;
    }
    
    static FL_UInt64 combineBits(FL_UInt64 hi, FL_UInt64 lo)
    {
        return (hi << 0x20) | highBits(lo);
    }
    
    friend SuperPrecision operator * (const SuperPrecision& lhs, const SuperPrecision& rhs)
    {
        // N.B. Overflow behaviour is undefined for multiplication (it is assumed you are using values well within range for the purpose)
        
        FL_UInt64 c0 = 0;
        FL_UInt64 c1 = 0;
        FL_UInt64 c2 = 0;
        FL_UInt64 c3 = 0;
        FL_UInt64 c4 = 0;
        FL_UInt64 t1, hi, md, lo;
        
        // Split both into 4 x 32 bits stored within 64 bit integers
        
        FL_UInt64 a1 = highBits(lhs.mInt);
        FL_UInt64 a2 = lowBits(lhs.mInt);
        FL_UInt64 a3 = highBits(lhs.mFracHi);
        FL_UInt64 a4 = lowBits(lhs.mFracHi);
        FL_UInt64 a5 = highBits(lhs.mFracLo);
        FL_UInt64 a6 = lowBits(lhs.mFracLo);
        
        FL_UInt64 b1 = highBits(rhs.mInt);
        FL_UInt64 b2 = lowBits(rhs.mInt);
        FL_UInt64 b3 = highBits(rhs.mFracHi);
        FL_UInt64 b4 = lowBits(rhs.mFracHi);
        FL_UInt64 b5 = highBits(rhs.mFracLo);
        FL_UInt64 b6 = lowBits(rhs.mFracLo);
        
        FL_UInt64 a1b1 = a1 * b1;
        FL_UInt64 a1b2 = a1 * b2;
        FL_UInt64 a1b3 = a1 * b3;
        FL_UInt64 a1b4 = a1 * b4;
        FL_UInt64 a1b5 = a1 * b5;
        FL_UInt64 a1b6 = a1 * b6;
        
        FL_UInt64 a2b1 = a2 * b1;
        FL_UInt64 a2b2 = a2 * b2;
        FL_UInt64 a2b3 = a2 * b3;
        FL_UInt64 a2b4 = a2 * b4;
        FL_UInt64 a2b5 = a2 * b5;
        FL_UInt64 a2b6 = a2 * b6;
        
        FL_UInt64 a3b1 = a3 * b1;
        FL_UInt64 a3b2 = a3 * b2;
        FL_UInt64 a3b3 = a3 * b3;
        FL_UInt64 a3b4 = a3 * b4;
        FL_UInt64 a3b5 = a3 * b5;
        FL_UInt64 a3b6 = a3 * b6;
        
        FL_UInt64 a4b1 = a4 * b1;
        FL_UInt64 a4b2 = a4 * b2;
        FL_UInt64 a4b3 = a4 * b3;
        FL_UInt64 a4b4 = a4 * b4;
        FL_UInt64 a4b5 = a4 * b5;
        FL_UInt64 a4b6 = a4 * b6;
        
        FL_UInt64 a5b1 = a5 * b1;
        FL_UInt64 a5b2 = a5 * b2;
        FL_UInt64 a5b3 = a5 * b3;
        FL_UInt64 a5b4 = a5 * b4;
        FL_UInt64 a5b5 = a5 * b5;
        FL_UInt64 a5b6 = a5 * b6;
        
        FL_UInt64 a6b1 = a6 * b1;
        FL_UInt64 a6b2 = a6 * b2;
        FL_UInt64 a6b3 = a6 * b3;
        FL_UInt64 a6b4 = a6 * b4;
        FL_UInt64 a6b5 = a6 * b5;
        FL_UInt64 a6b6 = a6 * b6;

        // Sub carry bits

        t1 = a6b6;
        sumWithCarry(t1, c0, (a5b6 << 0x20));
        sumWithCarry(t1, c0, (a6b5 << 0x20));

        // Lowest carry bits
        
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
        
        lo = a2b6;
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
        
        md = a2b4;
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
        
        hi = a2b2;
        sumWithCarry(hi, c4, a1b3);
        sumWithCarry(hi, c4, a3b1);
        sumWithCarry(hi, c4, combineBits(a1b2, a1b4));
        sumWithCarry(hi, c4, combineBits(a2b1, a4b1));
        sumWithCarry(hi, c4, highBits(a2b3) + highBits(a3b2));
        sumWithCarry(hi, c4, c3);
        
        // Do overflow
        
        if (a1b1 | highBits(a1b2) | highBits(a2b1) | c4)
            lo = md = hi = 0xFFFFFFFFULL;
        
        return SuperPrecision(hi, md, lo);
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
        md = subWithCarry(&lo, lhs.mFracLo, rhs.mFracLo) ? --md : md;
        
        return SuperPrecision(hi, md, lo);
    }

    
private:
    
    // Single 64 bit add/subtract with carry bit
    
    static inline void sumWithCarry(FL_Internal_UInt64& sum, FL_Internal_UInt64& carry, const FL_Internal_UInt64& add)
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
        
        mInt = absVal;
        mFrac = ((absVal - floor(absVal)) * 18446744073709551616.0);
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

    static inline bool addWithCarry(FL_Internal_UInt64 *result, const FL_Internal_UInt64& a, const FL_Internal_UInt64& b)
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
    
    static FL_UInt64 lowBits(FL_UInt64 a)
    {
        return a & 0xFFFFFFFF;
    }
    
    static FL_UInt64 highBits(FL_UInt64 a)
    {
        return a >> 0x20;
    }
    
    static FL_UInt64 combineBits(FL_UInt64 hi, FL_UInt64 lo)
    {
        return (hi << 0x20) | highBits(lo);
    }
    
    friend FrameLib_FixedPoint operator * (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        // N.B. Overflow behaviour is undefined for multiplication (it is assumed you are using values well within range for the purpose)
        
        FL_UInt64 c1, c2, c3;
        FL_UInt64 t1, hi, lo;
        
        // Split both into 4 x 32 bits stored within 64 bit integers
        
        FL_UInt64 a1 = highBits(lhs.mInt);
        FL_UInt64 a2 = lowBits(lhs.mInt);
        FL_UInt64 a3 = highBits(lhs.mFrac);
        FL_UInt64 a4 = lowBits(lhs.mFrac);
        
        FL_UInt64 b1 = highBits(rhs.mInt);
        FL_UInt64 b2 = lowBits(rhs.mInt);
        FL_UInt64 b3 = highBits(rhs.mFrac);
        FL_UInt64 b4 = lowBits(rhs.mFrac);
        
        FL_UInt64 a1b1 = a1 * b1;
        FL_UInt64 a1b2 = a1 * b2;
        FL_UInt64 a1b3 = a1 * b3;
        FL_UInt64 a1b4 = a1 * b4;
        
        FL_UInt64 a2b1 = a2 * b1;
        FL_UInt64 a2b2 = a2 * b2;
        FL_UInt64 a2b3 = a2 * b3;
        FL_UInt64 a2b4 = a2 * b4;
        
        FL_UInt64 a3b1 = a3 * b1;
        FL_UInt64 a3b2 = a3 * b2;
        FL_UInt64 a3b3 = a3 * b3;
        FL_UInt64 a3b4 = a3 * b4;
        
        FL_UInt64 a4b1 = a4 * b1;
        FL_UInt64 a4b2 = a4 * b2;
        FL_UInt64 a4b3 = a4 * b3;
        FL_UInt64 a4b4 = a4 * b4;
        
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
    
        if (a1b1 | highBits(a1b2) | highBits(a2b1) | c3)
            lo = hi = 0xFFFFFFFFULL;
        
        return FrameLib_FixedPoint(hi, lo);
    }
    
    FrameLib_FixedPoint& operator *= (const FrameLib_FixedPoint& rhs)
    {
        *this = *this * rhs;
        return *this;
    }
    
    // Division
    
    friend FrameLib_FixedPoint operator / (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        // Get a decent reciprocal estimate
        //double thing = 1.0 / (double) rhs;
        //FrameLib_FixedPoint recip = 1.0 / (double) rhs;
        //FrameLib_FixedPoint test1 = rhs * recip;
        //FrameLib_FixedPoint test2 = (double) rhs * thing;

        FrameLib_FixedPoint recipEst = 1.0 / (double) rhs;
        SuperPrecision recip = SuperPrecision(recipEst.mInt, recipEst.mFrac, 0);
        SuperPrecision rhsSP = SuperPrecision(rhs.mInt, rhs.mFrac, 0);
        
        // Newton-Raphson
        
        for (int i = 0; i < 3; i++)
            recip *= (SuperPrecision(2, 0, 0) - (rhsSP * recip));
        
        SuperPrecision result = SuperPrecision(lhs.mInt, lhs.mFrac, 0) * recip;
        FrameLib_FixedPoint resultLP = FrameLib_FixedPoint(result.intPart(), result.fracHiPart());
        resultLP = (result.fracLoPart() & 0x8000000000000000ULL) ? resultLP + FrameLib_FixedPoint(0, 1) : resultLP;
        
        SuperPrecision test3 = rhsSP * result;
        
        test3 = SuperPrecision(result.intPart(), result.fracHiPart(), 0) * rhsSP;

        FrameLib_FixedPoint test2 = resultLP * rhs;
        
        test2 = (resultLP - FrameLib_FixedPoint(0, 1)) * rhs;
        
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
